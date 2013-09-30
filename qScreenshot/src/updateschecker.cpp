/*
 * updateschecker.cpp
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

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include <QRegExp>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopServices>

#include "updateschecker.h"
#include "options.h"
#include "defines.h"

static const QString url = "http://qscreenshot.googlecode.com/svn/trunk/qScreenshot/src/defines.h";
static const QString downloadUrl = "http://code.google.com/p/qscreenshot/downloads/list";


UpdatesChecker::UpdatesChecker(bool autoCheck, QObject *parent)
	: QObject(parent)
	, manager_ (new QNetworkAccessManager(this))
	, progressDialog_(0)
	, autocheck_(autoCheck)
{
	Options *o = Options::instance();
	const QString proxyHost = o->getOption(constProxyHost).toString();
	if(!proxyHost.isEmpty()) {
		QNetworkProxy p(QNetworkProxy::HttpCachingProxy, proxyHost, o->getOption(constProxyPort).toInt()
				, o->getOption(constProxyUser).toString(), o->getOption(constProxyPass).toString());
		if(o->getOption(constProxyType).toString() == "socks") {
			p.setType(QNetworkProxy::Socks5Proxy);
		}
		manager_->setProxy(p);
	}

	connect(manager_, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	QNetworkRequest request;
	request.setUrl(QUrl(url));

	QNetworkReply* reply = manager_->get(request);

	if(!autocheck_) {
		QWidget *p = dynamic_cast<QWidget*>(parent);
		progressDialog_ = new QProgressDialog(p);
		progressDialog_->setWindowTitle(APP_NAME);
		progressDialog_->setWindowModality(Qt::WindowModal);
		progressDialog_->show();
		connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
	}
}

UpdatesChecker::~UpdatesChecker()
{
	delete progressDialog_;
	progressDialog_ = 0;
}

void UpdatesChecker::updateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	progressDialog_->setMaximum(bytesTotal);
	progressDialog_->setValue(bytesReceived);
}

void UpdatesChecker::replyFinished(QNetworkReply *reply)
{
	if(progressDialog_)
		progressDialog_->hide();

	if(reply->error() != QNetworkReply::NoError) {
		showError(tr("Error while checking for updates: %1").arg(reply->errorString()));
	}
	else {
		QString data = reply->readAll();
		const QRegExp re("#define APP_VERSION \"([0-9\\.]+)\"");
		if(re.indexIn(data) != -1) {
			const QString ver = re.cap(1);
			const QString curVer = Options::instance()->getOption(constVersionOption).toString();
			if(ver != curVer) {
				int rez = QMessageBox::question(0, QString(APP_NAME),
						tr("New version is available") + "\n" +
						tr("Do you want to go to the download page?"),
						QMessageBox::Yes | QMessageBox::No);

				if(rez == QMessageBox::Yes) {
					QDesktopServices::openUrl(QUrl(downloadUrl));
				}
			}
			else {
				if(!autocheck_)
					QMessageBox::information(0, APP_NAME, tr("There is no updates found."), QMessageBox::Ok);
			}
		}
		else {
			showError(tr("Can't find information about version."));
		}
	}

	reply->close();
	reply->deleteLater();
	this->deleteLater();
}

void UpdatesChecker::showError(const QString &error)
{
	if(!autocheck_)
		QMessageBox::warning(0, APP_NAME, error, QMessageBox::Ok);
}
