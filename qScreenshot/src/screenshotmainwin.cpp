/*
 * screenshotmainwin.cpp
 * Copyright (C) 2009-2013  Khryukin Evgeny
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <QDesktopWidget>
#include <QBuffer>
#include <QFileDialog>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QDateTime>
#include <QTimer>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QClipboard>
#include <QPushButton>


#include "screenshotmainwin.h"
#include "server.h"
#include "screenshotoptions.h"
#include "options.h"
#include "iconset.h"
#include "optionsdlg.h"
#include "defines.h"
#include "aboutdlg.h"
#include "proxysettingsdlg.h"
#include "updateschecker.h"
#include "historydlg.h"
#include "grabareawidget.h"
#include "ui_screenshot.h"

#include "qxtwindowsystem.h"

#define PROTOCOL_FTP "ftp"
#define PROTOCOL_HTTP "http"
#define MAX_HISTORY_SIZE 10


//----------------------------------------------
//-----------Screenshot-------------------------
//----------------------------------------------
ScreenshotMainWin::ScreenshotMainWin()
	: QMainWindow()
	, modified(false)
	, lastFolder(QDir::home().absolutePath())
	, proxy_(new Proxy())
	, grabAreaWidget_(0)
	, so_(0)
	, ui_(new Ui::Screenshot)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui_->setupUi(this);

	updateWidgets(false);
	ui_->frame->setVisible(false);

	refreshSettings();
	setProxy();
	history_ = Options::instance()->getOption(constHistory).toStringList();

	ui_->lb_pixmap->setToolBar(ui_->tb_bar);

	Iconset* icoHost = Iconset::instance();
	ui_->pb_upload->setIcon(icoHost->getIcon("upload"));
	ui_->pb_cancel->setIcon(icoHost->getIcon("cancel"));
	ui_->pb_open->setIcon(icoHost->getIcon("browse"));
	ui_->pb_save->setIcon(icoHost->getIcon("download"));
	ui_->pb_print->setIcon(icoHost->getIcon("print"));
	ui_->pb_new_screenshot->setIcon(icoHost->getIcon("screenshot"));
	ui_->tb_copyUrl->setIcon(icoHost->getIcon("copy"));

	ui_->pb_save->setShortcut(QKeySequence("Ctrl+s"));
	ui_->pb_upload->setShortcut(QKeySequence("Ctrl+u"));
	ui_->pb_open->setShortcut(QKeySequence("Ctrl+o"));
	ui_->pb_print->setShortcut(QKeySequence("Ctrl+p"));
	ui_->pb_new_screenshot->setShortcut(QKeySequence("Ctrl+n"));

	connectMenu();
	setupStatusBar();

	QByteArray state = Options::instance()->getOption(constState).toByteArray();
	if(!state.isEmpty())
		restoreState(state);

	connect(ui_->pb_save, SIGNAL(clicked()), this, SLOT(saveScreenshot()));
	connect(ui_->pb_upload, SIGNAL(clicked()), this, SLOT(uploadScreenshot()));
	connect(ui_->pb_print, SIGNAL(clicked()), this, SLOT(printScreenshot()));
	connect(ui_->pb_new_screenshot, SIGNAL(clicked()), this, SLOT(newScreenshot()));
	connect(ui_->pb_cancel,SIGNAL(clicked()),this,SLOT(cancelUpload()));
	connect(ui_->pb_open, SIGNAL(clicked()), this, SLOT(openImage()));
	connect(ui_->lb_pixmap, SIGNAL(adjusted()), this, SLOT(pixmapAdjusted()));
	connect(ui_->lb_pixmap, SIGNAL(settingsChanged(QString,QVariant)), SLOT(settingsChanged(QString, QVariant)));
	connect(ui_->lb_pixmap, SIGNAL(modified(bool)), this, SLOT(setModified(bool)));
	connect(ui_->tb_copyUrl, SIGNAL(clicked()), this, SLOT(copyUrl()));

	setWindowIcon(icoHost->getIcon("screenshot"));

	ui_->lb_pixmap->installEventFilter(this);
}

ScreenshotMainWin::~ScreenshotMainWin()
{
	qDeleteAll(servers);
	servers.clear();
	saveGeometry();
	delete ui_;
	delete proxy_;
	delete grabAreaWidget_;
	delete so_;
}

void ScreenshotMainWin::connectMenu()
{
	connect(ui_->actionAbout_Qt, SIGNAL(triggered()), SLOT(aboutQt()));
	connect(ui_->actionHome_page, SIGNAL(triggered()), SLOT(doHomePage()));
	connect(ui_->actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(ui_->actionHistory, SIGNAL(triggered()), SLOT(doHistory()));
	connect(ui_->actionNew_Screenshot, SIGNAL(triggered()), SLOT(newScreenshot()));
	connect(ui_->actionOpen_Image, SIGNAL(triggered()), SLOT(openImage()));
	connect(ui_->actionOptions, SIGNAL(triggered()), SLOT(doOptions()));
	connect(ui_->actionPrint, SIGNAL(triggered()), SLOT(printScreenshot()));
	connect(ui_->actionProxy_Settings, SIGNAL(triggered()), SLOT(doProxySettings()));
	connect(ui_->actionSave, SIGNAL(triggered()), SLOT(saveScreenshot()));
	connect(ui_->actionUpload, SIGNAL(triggered()), SLOT(uploadScreenshot()));
	connect(ui_->actionAbout, SIGNAL(triggered()), SLOT(doAbout()));
	connect(ui_->actionCheck_for_updates, SIGNAL(triggered()), SLOT(doCheckUpdates()));
}

void ScreenshotMainWin::action()
{
	int action = Options::instance()->getOption(constDefaultAction, Desktop).toInt();
	switch(action) {
	case Area:
		captureArea(0);
		break;
	case Window:
		shootWindow();
		break;
	case Desktop:
	default:
		shootScreen();
		break;
	}
}

void ScreenshotMainWin::setupStatusBar()
{
	QStatusBar *sb = statusBar();
	sbLbSize = new QLabel;
	sbLbSize->setAlignment(Qt::AlignRight);
	sbLbSize->setTextInteractionFlags(Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse);
	sb->addPermanentWidget(sbLbSize);
}

void ScreenshotMainWin::updateStatusBar()
{
	const QSize s = ui_->lb_pixmap->getPixmap().size();
	QBuffer buffer;
	buffer.open( QBuffer::ReadWrite );
	ui_->lb_pixmap->getPixmap().save( &buffer , format.toAscii() );
	const qint64 size = buffer.size();
	sbLbSize->setText(tr("Size: %1x%2px; %3 bytes").arg(s.width()).arg(s.height()).arg(size));
//	sbLbSize->setMaximumWidth( QFontMetrics( sbLbSize->font() ).width( sbLbSize->text() ) + 10 );
}

void ScreenshotMainWin::aboutQt()
{
	qApp->aboutQt();
}

void ScreenshotMainWin::doHomePage()
{
	QDesktopServices::openUrl(QUrl("http://code.google.com/p/qscreenshot/"));
}

void ScreenshotMainWin::updateWidgets(bool vis)
{
	ui_->progressBar->setVisible(vis);
	ui_->pb_cancel->setVisible(vis);
	ui_->cb_servers->setEnabled(!vis);
	ui_->pb_upload->setEnabled(!vis);
}

void ScreenshotMainWin::setServersList(const QStringList& l)
{
	ui_->cb_servers->clear();
	qDeleteAll(servers);
	servers.clear();
	ui_->cb_servers->setEnabled(false);
	ui_->pb_upload->setEnabled(false);
	foreach(QString settings, l) {
		if(settings.isEmpty()) {
			continue;
		}
		Server *s = new Server();
		s->setFromString(settings);
		servers.append(s);
		ui_->cb_servers->addItem(s->displayName());
	}
	if(servers.size() > 0) {
		ui_->cb_servers->setEnabled(true);
		ui_->pb_upload->setEnabled(true);
	}
}

void ScreenshotMainWin::setProxy()
{
	Options *o = Options::instance();
	proxy_->host = o->getOption(constProxyHost).toString();
	proxy_->port = o->getOption(constProxyPort).toInt();
	proxy_->user = o->getOption(constProxyUser).toString();
	proxy_->pass = o->getOption(constProxyPass).toString();
	proxy_->type = o->getOption(constProxyType).toString();
}

void ScreenshotMainWin::openImage()
{
	QString fileName = QFileDialog::getOpenFileName(0,tr("Open Image"), lastFolder,tr("Images (*.png *.gif *.jpg *.jpeg *.ico)"));
	if(!fileName.isEmpty()) {
		setImagePath(fileName);
		QFileInfo fi(fileName);
		lastFolder = fi.absoluteDir().path();
		settingsChanged(constLastFolder, lastFolder);
		updateScreenshotLabel();
		bringToFront();
		setModified(false);
	}
}

void ScreenshotMainWin::setImagePath(const QString& path)
{
	originalPixmap = QPixmap(path);
	updateScreenshotLabel();
}

void ScreenshotMainWin::updateScreenshotLabel()
{
	ui_->lb_pixmap->setPixmap(originalPixmap);
	updateStatusBar();
}

void ScreenshotMainWin::pixmapAdjusted()
{
	updateStatusBar();

	if(windowState() & Qt::WindowMaximized)
		return;

	const QSize s = ui_->lb_pixmap->size() + QSize(15,20); //хак, для красивого уменьшения размера главного окна
	const QSize desktopSize = qApp->desktop()->availableGeometry(this).size();
	if(s.height() > desktopSize.height() || s.width() > desktopSize.width())
		resize(desktopSize);
	else {
		ui_->scrollArea->setMinimumSize(s);
		adjustSize();
		QTimer::singleShot(100, this, SLOT(fixSizes())); // необходимо время, чтобы ресайзить главное окно
	}
}

void ScreenshotMainWin::fixSizes()
{
	ui_->scrollArea->setMinimumSize(0,0);
}

void ScreenshotMainWin::setModified(bool m)
{
	modified = m;
}

void ScreenshotMainWin::printScreenshot()
{
	QPrinter p;
	QPrintDialog *pd = new QPrintDialog(&p, this);
	if(pd->exec() == QDialog::Accepted && p.isValid()) {
		QPainter painter;
		painter.begin(&p); 
		QPixmap pix = ui_->lb_pixmap->getPixmap();
		const QSize size = p.pageRect().size();
		if(pix.size().height() > size.height() || pix.size().width() > size.width()) {
			pix = pix.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}
		painter.drawPixmap(0, 0, pix);
		painter.end();
	}
	delete pd;
}

void ScreenshotMainWin::cancelUpload()
{
	if(manager) {
		manager->disconnect();
		manager->deleteLater();
	}
	updateWidgets(false);
}

void ScreenshotMainWin::bringToFront()
{
	Options* o = Options::instance();
	int x = o->getOption(constWindowX, 0).toInt();
	int y = o->getOption(constWindowY, 0).toInt();
	int h = o->getOption(constWindowHeight, 600).toInt();
	int w = o->getOption(constWindowWidth, 800).toInt();
	bool max = o->getOption(constWindowState, true).toBool();

	resize(w, h);
	move(x, y);
	if(max)
		showMaximized();
	else
		showNormal();

	raise();
	activateWindow();
	update();
}

void ScreenshotMainWin::newScreenshot()
{
	so_ = new ScreenshotOptions(Options::instance()->getOption(constDelay).toInt());
	connect(so_, SIGNAL(captureArea(int)), SLOT(captureArea(int)));
	connect(so_, SIGNAL(captureWindow(int)), this, SLOT(captureWindow(int)));
	connect(so_, SIGNAL(captureDesktop(int)), SLOT(captureDesktop(int)));
	connect(so_, SIGNAL(screenshotCanceled()), SLOT(screenshotCanceled()));
	saveGeometry();
	setWindowState(Qt::WindowMinimized);
	ui_->pb_new_screenshot->setEnabled(false);
	so_->show();
	so_->raise();
	so_->activateWindow();
}

void ScreenshotMainWin::screenshotCanceled()
{
	ui_->pb_new_screenshot->setEnabled(true);
	if(!isHidden())
		bringToFront();
}

void ScreenshotMainWin::refreshWindow()
{
	if(autoSave) {
		saveScreenshot();
	}
	else {
		ui_->pb_new_screenshot->setEnabled(true);
		ui_->frame->setVisible(false);
		updateScreenshotLabel();
		bringToFront();
		setModified(false);
	}
}

void ScreenshotMainWin::captureArea(int delay)
{
	grabAreaWidget_ = new GrabAreaWidget();
	if(grabAreaWidget_->exec() == QDialog::Accepted) {
		QTimer::singleShot(delay*1000, this, SLOT(shootArea()));
	}
	else {
		delete grabAreaWidget_;
		grabAreaWidget_ = 0;
		qApp->desktop()->repaint();
		refreshWindow();
	}
}

void ScreenshotMainWin::shootArea()
{
	if(!grabAreaWidget_) {
		return;
	}
	const QRect rect = grabAreaWidget_->getRect();
	if(rect.isValid()) {
		qApp->desktop()->repaint();
		qApp->beep();
		originalPixmap = QPixmap::grabWindow(QApplication::desktop()->winId(), rect.x(), rect.y(), rect.width(), rect.height());
	}

	delete grabAreaWidget_;
	grabAreaWidget_ = 0;

	refreshWindow();
}

void ScreenshotMainWin::captureWindow(int delay)
{
	QTimer::singleShot(delay*1000, this, SLOT(shootWindow()));
}

void ScreenshotMainWin::shootWindow()
{	
	shoot(QxtWindowSystem::activeWindow());
}

void ScreenshotMainWin::captureDesktop(int delay)
{
	QTimer::singleShot(delay*1000, this, SLOT(shootScreen()));
}

void ScreenshotMainWin::shootScreen()
{
	shoot(QApplication::desktop()->winId());
}

void ScreenshotMainWin::shoot(WId id)
{
	qApp->beep();
	originalPixmap = QPixmap::grabWindow(id);

	refreshWindow();
}

void ScreenshotMainWin::saveScreenshot()
{
	ui_->pb_save->setEnabled(false);
	QString fileName;
	QString initialFileName = tr("/%1.").arg(QDateTime::currentDateTime().toString(fileNameFormat)) + format;
	if(autoSave) {
		fileName = autosaveFolder + initialFileName;
	}
	else {
		originalPixmap = ui_->lb_pixmap->getPixmap();
		QString initialPath = lastFolder + initialFileName;
		fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
								   initialPath,
								   tr("%1 Files (*.%2);;All Files (*)")
								   .arg(format.toUpper())
								   .arg(format));
	}

	if (!fileName.isEmpty()) {
		originalPixmap.save(fileName, format.toLatin1());

		if(!autoSave) {
			QFileInfo fi(fileName);
			lastFolder = fi.absoluteDir().path();
			settingsChanged(constLastFolder, lastFolder);
		}
		else {
			emit screenshotSaved(fileName);
		}
	}
	ui_->pb_save->setEnabled(true);
	modified = false;
}

void ScreenshotMainWin::copyUrl()
{
	QString url = ui_->lb_url->text();
	if(!url.isEmpty()) {
		QRegExp re("<a href=\".+\">([^<]+)</a>");
		if(re.indexIn(url) != -1) {
			url = re.cap(1);
			qApp->clipboard()->setText(url);
		}
	}
}

void ScreenshotMainWin::dataTransferProgress(qint64 done, qint64 total)
{
	ui_->progressBar->setMaximum(total);
	ui_->progressBar->setValue(done);
}

void ScreenshotMainWin::uploadScreenshot()
{
	if(!ui_->cb_servers->isEnabled())
		return;

	int index = ui_->cb_servers->currentIndex();
	if(index == -1 || servers.size() <= index)
		return;

	Server *s = servers.at(index);
	if(!s)
		return;

	QString scheme = QUrl(s->url()).scheme();
	ui_->pb_upload->setEnabled(false);
	ui_->pb_cancel->setVisible(true);
	ui_->cb_servers->setEnabled(false);

	originalPixmap = ui_->lb_pixmap->getPixmap();

	if (scheme.toLower() == QLatin1String(PROTOCOL_FTP)) {
		uploadFtp();
	}
	else if (scheme.toLower() == QLatin1String(PROTOCOL_HTTP)) {
		uploadHttp();
	}
	else
		cancelUpload();
}

void ScreenshotMainWin::uploadFtp()
{
	ba.clear();
	QBuffer buffer( &ba );
	buffer.open( QBuffer::ReadWrite );
	originalPixmap.save( &buffer , format.toAscii() );

	QString fileName = tr("%1.").arg(QDateTime::currentDateTime().toString(fileNameFormat)) + format;

	QFileInfo fi(fileName);
	fileName = fi.fileName();

	Server *s = servers.at(ui_->cb_servers->currentIndex());
	if(!s)
		cancelUpload();

	QUrl u;
	u.setPort(21);
	u.setUrl(s->url(), QUrl::TolerantMode);
	u.setUserName(s->userName());
	u.setPassword(s->password());

	if(manager) {
		delete manager;
	}

	manager = new QNetworkAccessManager(this);
	if(s->useProxy() && !proxy_->host.isEmpty()) {
		QNetworkProxy p = QNetworkProxy(QNetworkProxy::HttpCachingProxy, proxy_->host, proxy_->port, proxy_->user, proxy_->pass);
		if(proxy_->type == "socks")
			p.setType(QNetworkProxy::Socks5Proxy);
		manager->setProxy(p);
	}

	QString path = u.path();
	if(path.right(1) != "/")
		path += "/";
	u.setPath(path+fileName);

	ui_->progressBar->setValue(0);
	ui_->progressBar->show();
	ui_->frame->setVisible(false);

	QNetworkReply *reply = manager->put(QNetworkRequest(u), ba);

	connect(reply, SIGNAL(uploadProgress(qint64 , qint64)), this, SLOT(dataTransferProgress(qint64 , qint64)));
	//connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(finished()), this, SLOT(ftpReplyFinished()));

	modified = false;
}

void ScreenshotMainWin::uploadHttp()
{
	ba.clear();
	QUrl u;

	const QString boundary = "AaB03x";
	const QString filename = tr("%1.").arg(QDateTime::currentDateTime().toString(fileNameFormat)) + format;

	Server *s = servers.at(ui_->cb_servers->currentIndex());
	if(!s)
		cancelUpload();

	if (s->servPostdata().length()>0) {
		foreach (QString poststr, s->servPostdata().split("&")) {
			QStringList postpair = poststr.split("=");
			if(postpair.count() < 2)
				continue;
			ba.append("--" + boundary + "\r\n");
			ba.append("Content-Disposition: form-data; name=\"" + postpair[0] + "\"\r\n");
			ba.append("\r\n" + postpair[1] + "\r\n");
		}
	}

	ba.append("--" + boundary + "\r\n");
	ba.append("Content-Disposition: form-data; name=\"" + s->servFileinput() + "\"; filename=\"" + filename.toUtf8() + "\"\r\n");
	ba.append("Content-Transfer-Encoding: binary\r\n");
	ba.append(QString("Content-Type: image/%1\r\n")
		  .arg(format == "jpg" ? "jpeg" : format) // FIXME!!!!! жуткий костыль, но что поделаешь
		  .toUtf8());
	ba.append("\r\n");

	QByteArray a;
	QBuffer buffer(&a);
	buffer.open( QBuffer::ReadWrite );
	originalPixmap.save( &buffer , format.toAscii() );
	ba.append(a);

	ba.append("\r\n--" + boundary + "--\r\n");

	if(manager) {
		delete manager;
	}

	manager = new QNetworkAccessManager(this);

	if(s->useProxy() && !proxy_->host.isEmpty()) {
		QNetworkProxy p = QNetworkProxy(QNetworkProxy::HttpCachingProxy, proxy_->host, proxy_->port, proxy_->user, proxy_->pass);
		if(proxy_->type == "socks")
			p.setType(QNetworkProxy::Socks5Proxy);
		manager->setProxy(p);
	}

	QNetworkRequest netreq;
	netreq.setUrl(QUrl(s->url()));

	netreq.setRawHeader("User-Agent", "qScreenshot");
	netreq.setRawHeader("Content-Type", "multipart/form-data, boundary=" + boundary.toLatin1());
	netreq.setRawHeader("Cache-Control", "no-cache");
	netreq.setRawHeader("Accept", "*/*");
	netreq.setRawHeader("Content-Length", QString::number(ba.length()).toLatin1());

	ui_->progressBar->setValue(0);
	ui_->progressBar->show();
	ui_->frame->setVisible(false);

	QNetworkReply* reply = manager->post(netreq, ba);
	connect(reply, SIGNAL(uploadProgress(qint64 , qint64)), this, SLOT(dataTransferProgress(qint64 , qint64)));
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpReplyFinished(QNetworkReply*)));
	modified = false;
}

void ScreenshotMainWin::ftpReplyFinished()
{
	QNetworkReply *reply = (QNetworkReply*)sender();
	ui_->frame->setVisible(true);
	if(reply->error() == QNetworkReply::NoError) {
		const QString url = reply->url().toString(QUrl::RemoveUserInfo | QUrl::StripTrailingSlash);
		ui_->lb_url->setText(QString("<a href=\"%1\">%1</a>").arg(url));
		history_.push_front(url);
		if(history_.size() > MAX_HISTORY_SIZE) {
			history_.removeLast();
		}
		settingsChanged(constHistory, history_);
	}
	else {
		ui_->lb_url->setText(reply->errorString());
	}
	reply->close();
	reply->deleteLater();
	updateWidgets(false);
}

void ScreenshotMainWin::httpReplyFinished(QNetworkReply *reply)
{
	if(reply->error() != QNetworkReply::NoError) {
		ui_->frame->setVisible(true);
		ui_->lb_url->setText(reply->errorString());
		updateWidgets(false);
		reply->close();
		reply->deleteLater();
		return;
	}

	const QString loc = reply->rawHeader("Location");
	const QString refresh = reply->rawHeader("refresh");
	if (!loc.isEmpty()) {
		newRequest(reply, loc);
	}
	else if(!refresh.isEmpty() && refresh.contains("url=", Qt::CaseInsensitive)) {
		QStringList tmp = refresh.split("=");
		if(tmp.size() > 1) {
			newRequest(reply, tmp.last());
		}
	}
	else {
		Server *s = servers.at(ui_->cb_servers->currentIndex());
		QString page = reply->readAll();

//
//		//Код нужен для анализа html и нахождения ссылки на картинку
//		QFile f(QDir::home().absolutePath() + "/page.html");
//		if(f.open(QIODevice::WriteOnly)) {
//			QTextStream out(&f);
//			out << page;
//			f.close();
//		}
//

		QRegExp rx(s->servRegexp());
		ui_->frame->setVisible(true);
		if (rx.indexIn(page) != -1) {
			QString imageurl = rx.cap(1);
			ui_->lb_url->setText(QString("<a href=\"%1\">%1</a>").arg(imageurl));
			history_.push_front(imageurl);
			if(history_.size() > MAX_HISTORY_SIZE) {
				history_.removeLast();
			}
			settingsChanged(constHistory, history_);
		}
		else
			ui_->lb_url->setText(tr("Can't parse URL (Reply URL: <a href=\"%1\">%1</a>)").arg(reply->url().toString()));

		updateWidgets(false);	
	}
	reply->close();
	reply->deleteLater();
}

void ScreenshotMainWin::newRequest(const QNetworkReply *const old, const QString &link)
{
	if(!manager || !old || link.isEmpty())
		return;

	QUrl netrequrl(link);
	if (netrequrl.host().isEmpty())
		netrequrl = QUrl("http://" + old->url().encodedHost() + link);
	QNetworkRequest netreq(netrequrl);

	ui_->progressBar->setValue(0);
	QNetworkReply* reply = manager->get(netreq);
	connect(reply, SIGNAL(uploadProgress(qint64 , qint64)), this, SLOT(dataTransferProgress(qint64 , qint64)));
}

void ScreenshotMainWin::closeEvent(QCloseEvent *e)
{
	e->ignore();
	if(modified) {
		int ret = QMessageBox::question(this, tr("Close Screenshot"), tr("Are your sure?"),
						QMessageBox::Ok | QMessageBox::Cancel);
		if(ret != QMessageBox::Ok) {
			return;
		}
	}
	saveGeometry();
	hide();
}

void ScreenshotMainWin::settingsChanged(const QString &option, const QVariant &value)
{
	Options::instance()->setOption(option, value);
}

void ScreenshotMainWin::doOptions()
{
	OptionsDlg od(this);
	if(od.exec() == QDialog::Accepted) {
		refreshSettings();
	}
}

void ScreenshotMainWin::doHistory()
{
	new HistoryDlg(history_, this);
}

void ScreenshotMainWin::doProxySettings()
{
	ProxySettingsDlg ps(this);
	ps.setProxySettings(*proxy_);
	if(ps.exec() == QDialog::Accepted) {
		Options *o = Options::instance();
		Proxy prox = ps.getSettings();
		o->setOption(constProxyHost, prox.host);
		o->setOption(constProxyPort, prox.port);
		o->setOption(constProxyUser, prox.user);
		o->setOption(constProxyPass, prox.pass);
		o->setOption(constProxyType, prox.type);
		setProxy();
	}
}

void ScreenshotMainWin::doAbout()
{
	new AboutDlg(this);
}

void ScreenshotMainWin::doCheckUpdates()
{
	new UpdatesChecker(this);
}

void ScreenshotMainWin::refreshSettings()
{
	Options* o = Options::instance();
	format = o->getOption(constFormat, format).toString();
	fileNameFormat = o->getOption(constFileName, fileNameFormat).toString();
	lastFolder = o->getOption(constLastFolder, lastFolder).toString();
	setServersList(o->getOption(constServerList).toStringList());
	autoSave = o->getOption(constAutosave, false).toBool();
	autosaveFolder = o->getOption(constAutosaveFolder, QDir::homePath()).toString();
}

void ScreenshotMainWin::saveGeometry()
{
	Options* o = Options::instance();
	o->setOption(constWindowState, QVariant(windowState() & Qt::WindowMaximized));
	o->setOption(constWindowX, x());
	o->setOption(constWindowY, y());
	o->setOption(constWindowWidth, width());
	o->setOption(constWindowHeight, height());
	o->setOption(constState, saveState());
}

bool ScreenshotMainWin::eventFilter(QObject *o, QEvent *e)
{
	if(o == ui_->lb_pixmap && e->type() == QEvent::MouseMove) {
		QMouseEvent *me = static_cast<QMouseEvent*>(e);
		if(me->buttons() == Qt::LeftButton) {
			QPoint pos = me->pos();
			ui_->scrollArea->ensureVisible(pos.x(), pos.y(), 10, 10);
		}
	}

	return QMainWindow::eventFilter(o, e);
}
