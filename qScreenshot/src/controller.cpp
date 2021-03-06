/*
 * controller.cpp
 * Copyright (C) 2011  Khryukin Evgeny
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

#include <QMessageBox>
#include <QMenu>
#include <QApplication>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>

#include "controller.h"
#include "screenshotmainwin.h"
#include "server.h"
#include "options.h"
#include "iconset.h"
#include "defines.h"
#include "shortcutmanager.h"
#include "translator.h"
#include "updateschecker.h"


static const QString imageShack = "ImageShack.us&split&http://www.imageshack.us/upload_api.php&split&&split&&split&a_username=...&a_password=...&split&fileupload&split&<image_link>(http://[^<]+)</image_link>&split&true";
static const QString radikal = "Radikal.ru&split&http://www.radikal.ru/action.aspx&split&&split&&split&upload=yes&split&F&split&<input\\s+id=\"input_link_1\"\\s+value=\"([^\"]+)\"&split&true";
static const QString pixacadem = "Pix.Academ.org&split&http://pix.academ.org/&split&&split&&split&action=upload_image&split&image&split&<div id='link'><a id=\"original\" href=\"(http[^\"]+)\"&split&true";
static const QString kachalka = "Kachalka.com&split&http://www.kachalka.com/upload.php&split&&split&&split&&split&userfile[]&split&name=\"option\" value=\"(http://www.kachalka.com/[^\"]+)\" /></td>&split&true";
static const QString flashtux = "Img.Flashtux.org&split&http://img.flashtux.org/index.php&split&&split&&split&postimg=1&split&filename&split&<br />Link: <a href=\"(http://img.flashtux.org/[^\"]+)\">&split&true";
static const QString smages = "Smages.com&split&http://smages.com/&split&&split&&split&&split&fileup&split&<div class=\"codex\"><a href=\"(http://smages.com/[^\"]+)\" target=\"_blank\">URL:</a></div>&split&true";
static const QString ompldr = "Omploader.org&split&http://ompldr.org/upload&split&&split&&split&&split&file1&split&<div class=\"left\">File:</div><div class=\"right\"><a href=\"[^\"]+\">(http://ompldr.org/[^\"]+)</a></div>&split&true";
static const QString ipicture = "Ipicture.ru&split&http://ipicture.ru/Upload/&split&&split&&split&method=file&split&userfile&split&value=\"(http://[^\"]+)\">&split&true";
static const QString rghost = "Rghost.ru&split&http://pion.rghost.ru/files&split&&split&&split&&split&file&split&value=\"(http://[^\"]+image.[^\"]+)\"&split&true";

static const QStringList staticHostsList = QStringList() << imageShack << pixacadem /*<< radikal*/
					 << kachalka << flashtux << smages /*<< ompldr*/ << ipicture
					 << rghost;


static bool isListContainsServer(const QString& server, const QStringList& servers)
{
	foreach(const QString& serv, servers) {
		if(serv.split(Server::splitString()).first() == server.split(Server::splitString()).first())
			return true;
	}
	return false;
}

static void updateServer(QStringList *const servers, const QString& serv)
{
	QStringList::iterator it = servers->begin();
	while(++it != servers->end()) {
		const QStringList tmpOld = (*it).split(Server::splitString());
		const QStringList tmpNew = serv.split(Server::splitString());
		if(tmpOld.first() == tmpNew.first()) {
			*it = serv;
		}
	}
}



//--------------------------------------
//------------------LangMenu-------------
//--------------------------------------
class LangMenu : public QMenu
{
	Q_OBJECT
public:
	LangMenu(const QStringList& actions, const QString& curTrans, const QString& title, QWidget* p) : QMenu(title, p)
	{
		foreach(const QString& action, actions) {
			QAction* act = new QAction(action, this);
			act->setCheckable(true);
			if(action == curTrans) {
				act->setChecked(true);
			}
			connect(act, SIGNAL(triggered(bool)), SLOT(actionTriggered(bool)));
			addAction(act);

		}
	}

	~LangMenu()
	{
	}

signals:
	void newTranslation(const QString&);

private slots:
	void actionTriggered(bool state)
	{
		QAction *action = static_cast<QAction*>(sender());
		if(!state) {
			action->setChecked(!state);
			return;
		}

		foreach(QAction* act, actions()) {
			if(act != action) {
				act->setChecked(false);
			}
		}

		emit newTranslation(action->text());
	}
};


//--------------------------------------
//------------------Controller----------
//--------------------------------------
Controller::Controller()
	: QObject()
	, screenshot(0)
	, trayMenu_(0)
{
	qApp->setWindowIcon(Iconset::instance()->getIcon("screenshot"));
	Options* o = Options::instance();
	QVariant vServers = o->getOption(constServerList);
	QVariant vVer = o->getOption(constVersionOption);

	QStringList servers = vServers.toStringList();
	foreach(const QString& host, staticHostsList) {
		if(!isListContainsServer(host, servers))
			servers.append(host);
	}

	if(!vVer.isValid() || vVer.toString() != APP_VERSION) {
//		foreach(const QString& host, staticHostsList) {
//			updateServer(&servers, host);
//		}

		updateServer(&servers, imageShack);
		doUpdate();

		o->setOption(constVersionOption, APP_VERSION);
	}

	o->setOption(constServerList, servers); //сохраняем обновленный список серверов

	if(!ShortcutManager::instance()->setShortcut(QKeySequence(o->getOption(constShortCut).toString()))) {
		QMessageBox::critical(0, tr("Error"), tr("Failed to register shortcut!"), QMessageBox::Ok);
	}
	
	screenshot = new ScreenshotMainWin();
	connect(screenshot, SIGNAL(screenshotSaved(QString)), SLOT(screenshotSaved(QString)));

	connect(ShortcutManager::instance(), SIGNAL(activated()), screenshot, SLOT(action()));

	buildTray();

	if(o->getOption(constAutocheckUpdates).toBool()) {
		QTimer::singleShot(5000, this, SLOT(autocheckUpdates()));
	}
}

Controller::~Controller()
{
	delete screenshot;
	delete trayMenu_;
	Options::reset();
	Iconset::reset();
	ShortcutManager::reset();
	Translator::reset();
}

void Controller::buildTray()
{
	trayIcon_ = new QSystemTrayIcon(this);
	trayIcon_->setIcon(Iconset::instance()->getIcon("screenshot"));
	trayIcon_->setToolTip(QString(APP_NAME) + " " + QString(APP_VERSION));
	connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	trayMenu_ = new QMenu();
	trayMenu_->addAction(tr("Grab screen"), screenshot, SLOT(captureDesktop()));
	trayMenu_->addAction(tr("Grab active window"), screenshot, SLOT(captureActiveWindow()));
	trayMenu_->addAction(tr("Grab area"), screenshot, SLOT(captureArea()));
	trayMenu_->addAction(tr("New Screenshot"), screenshot, SLOT(newScreenshot()));
	trayMenu_->addAction(tr("Open Image"), screenshot, SLOT(openImage()));
	trayMenu_->addSeparator();

	QMenu* settingsMenu = trayMenu_->addMenu(tr("Settings"));
	settingsMenu->addAction(tr("Options"), screenshot, SLOT(doOptions()));
	settingsMenu->addAction(tr("Proxy settings"), screenshot, SLOT(doProxySettings()));

	LangMenu *lang = new LangMenu(Translator::availableTranslations(),
				      Translator::instance()->currentTranslation(),
				      tr("Language"), settingsMenu);

	connect(lang, SIGNAL(newTranslation(QString)), SLOT(retranslate(QString)));
	settingsMenu->addMenu(lang);

	trayMenu_->addSeparator();
	trayMenu_->addAction(tr("Exit"), qApp, SLOT(quit()));

	trayIcon_->setContextMenu(trayMenu_);

	connect(trayIcon_, SIGNAL(messageClicked()), SLOT(trayMessageClicked()));
	trayIcon_->show();
    
#ifdef Q_WS_MAC    
    extern void qt_mac_set_dock_menu(QMenu *);
	qt_mac_set_dock_menu(trayMenu_);
#endif
}

void Controller::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::DoubleClick) {
		screenshot->captureDesktop();
	}
}

void Controller::doUpdate()
{
	// do some updates
	Options* o = Options::instance();


	if(o->getOption(constVersionOption) == QVariant::Invalid)
		o->setOption(constVersionOption, APP_VERSION);

	if(o->getOption(constAutosave) == QVariant::Invalid)
		o->setOption(constAutosave, false);

	if(o->getOption(constAutosaveFolder) == QVariant::Invalid)
		o->setOption(constAutosaveFolder, QDir::homePath());

	if(o->getOption(constAutocheckUpdates) == QVariant::Invalid)
		o->setOption(constAutocheckUpdates, true);

	if(o->getOption(constShortCut) == QVariant::Invalid)
		o->setOption(constShortCut, QVariant("Ctrl+Shift+S"));

	if(o->getOption(constFormat) == QVariant::Invalid)
		o->setOption(constFormat, QVariant("png"));

	if(o->getOption(constFileName) == QVariant::Invalid)
		o->setOption(constFileName, QVariant("pic-yyyyMMdd-hhmmss"));

	if(o->getOption(constDelay) == QVariant::Invalid)
		o->setOption(constDelay, QVariant(0));

	if(o->getOption(constDefaultAction) == QVariant::Invalid)
		o->setOption(constDefaultAction, QVariant(Desktop));

	if(o->getOption(constAutocheckUpdates) == QVariant::Invalid)
		o->setOption(constAutocheckUpdates, true);
}

void Controller::retranslate(const QString &trans)
{
	Translator::instance()->retranslate(trans);
	QMessageBox::information(0, tr("Translate"), tr("Restart application!"), QMessageBox::Ok);
}

void Controller::screenshotSaved(const QString &name)
{
	trayIcon_->showMessage(tr("Screenshot saved."), name, QSystemTrayIcon::Information, 5000);
}

void Controller::trayMessageClicked()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(Options::instance()->getOption(constAutosaveFolder).toString()));
}

void Controller::autocheckUpdates()
{
	new UpdatesChecker(true);
}

#include "controller.moc"
