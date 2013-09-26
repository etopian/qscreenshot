/*
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

#include "uploader.h"
#include "server.h"
#include "options.h"
#include "defines.h"
#include "common.h"

#include <QNetworkReply>
#include <QNetworkProxy>
#include <QBuffer>

Uploader::Uploader(Server *serv, QObject *parent)
	: QObject(parent)
	, manager_(new QNetworkAccessManager(this))
	, server_(serv)
{
	Options *o = Options::instance();
	const QString host = o->getOption(constProxyHost).toString();

	if(server_->useProxy() && !host.isEmpty()) {
		QNetworkProxy p = QNetworkProxy(QNetworkProxy::HttpCachingProxy, host,
						o->getOption(constProxyPort).toInt(),
						o->getOption(constProxyUser).toString(),
						o->getOption(constProxyPass).toString());
		if(o->getOption(constProxyType).toString() == "socks")
			p.setType(QNetworkProxy::Socks5Proxy);
		manager_->setProxy(p);
	}

	connect(manager_, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpReplyFinished(QNetworkReply*)));
}

Uploader::~Uploader()
{
}

void Uploader::uploadFtp(const QPixmap &pixmap)
{
	QBuffer buffer( &ba_ );
	buffer.open( QBuffer::ReadWrite );
	const QString format = Options::instance()->getOption(constFormat).toString();
	pixmap.save( &buffer , format.toLatin1() );

	const QString fileName = getFileName();

	QUrl u;
	u.setPort(21);
	u.setUrl(server_->url(), QUrl::TolerantMode);
	u.setUserName(server_->userName());
	u.setPassword(server_->password());

	QString path = u.path();
	if(path.right(1) != "/")
		path += "/";
	u.setPath(path+fileName);

	QNetworkReply *reply = manager_->put(QNetworkRequest(u), ba_);

	connect(reply, SIGNAL(uploadProgress(qint64 , qint64)), SIGNAL(uploadProgress(qint64 , qint64)));
	connect(reply, SIGNAL(finished()), this, SLOT(ftpReplyFinished()));
}

void Uploader::uploadHttp(const QPixmap &pixmap)
{
	static const QString boundary = "AaB03x";
	const QString filename = getFileName();

	if (server_->servPostdata().length()>0) {
		foreach (const QString& poststr, server_->servPostdata().split("&")) {
			QStringList postpair = poststr.split("=");
			if(postpair.count() < 2)
				continue;
			ba_.append("--" + boundary + "\r\n");
			ba_.append("Content-Disposition: form-data; name=\"" + postpair[0] + "\"\r\n");
			ba_.append("\r\n" + postpair[1] + "\r\n");
		}
	}

	ba_.append("--" + boundary + "\r\n");
	ba_.append("Content-Disposition: form-data; name=\"" + server_->servFileinput() + "\"; filename=\"" + filename.toUtf8() + "\"\r\n");
	ba_.append("Content-Transfer-Encoding: binary\r\n");
	const QString format = Options::instance()->getOption(constFormat).toString();
	ba_.append(QString("Content-Type: image/%1\r\n")
		  .arg(format == "jpg" ? "jpeg" : format) // FIXME!!!!! жуткий костыль, но что поделаешь
		  .toUtf8());
	ba_.append("\r\n");

	QByteArray a;
	QBuffer buffer(&a);
	buffer.open( QBuffer::ReadWrite );
	pixmap.save( &buffer , format.toLatin1() );
	ba_.append(a);

	ba_.append("\r\n--" + boundary + "--\r\n");

	QNetworkRequest netreq;
	netreq.setUrl(QUrl(server_->url()));

	netreq.setRawHeader("User-Agent", "qScreenshot");
	netreq.setRawHeader("Content-Type", "multipart/form-data, boundary=" + boundary.toLatin1());
	netreq.setRawHeader("Cache-Control", "no-cache");
	netreq.setRawHeader("Accept", "*/*");
	netreq.setRawHeader("Content-Length", QString::number(ba_.length()).toLatin1());

	QNetworkReply* reply = manager_->post(netreq, ba_);
	connect(reply, SIGNAL(uploadProgress(qint64 , qint64)), SIGNAL(uploadProgress(qint64 , qint64)));
}

void Uploader::ftpReplyFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	if(reply->error() == QNetworkReply::NoError) {
		const QString url = reply->url().toString(QUrl::RemoveUserInfo | QUrl::StripTrailingSlash);
		emit newUrl(QString("<a href=\"%1\">%1</a>").arg(url));
		emit addToHistory(url);
	}
	else {
		emit newUrl(reply->errorString());
	}

	reply->deleteLater();
	emit requestFinished();
	deleteLater();
}

void Uploader::httpReplyFinished(QNetworkReply *reply)
{
	reply->deleteLater();

	if(reply->error() != QNetworkReply::NoError) {
		emit newUrl(reply->errorString());
	}
	else {
		const QString loc = reply->rawHeader("Location");
		const QString refresh = reply->rawHeader("refresh");
		if (!loc.isEmpty()) {
			newRequest(reply, loc);
			return;
		}
		else if(!refresh.isEmpty() && refresh.contains("url=", Qt::CaseInsensitive)) {
			QStringList tmp = refresh.split("=");
			if(tmp.size() > 1) {
				newRequest(reply, tmp.last());
				return;
			}
		}
		else {
			const QString page = QString::fromUtf8(reply->readAll());

//			//Код нужен для анализа html и нахождения ссылки на картинку
//			QFile f(QDir::home().absolutePath() + "/page.html");
//			if(f.open(QIODevice::WriteOnly)) {
//				QTextStream out(&f);
//				out << page;
//				f.close();
//			}

			QRegExp rx(server_->servRegexp());
			if (rx.indexIn(page) != -1) {
				QString imageurl = rx.cap(1);
				emit newUrl(QString("<a href=\"%1\">%1</a>").arg(imageurl));
				emit addToHistory(imageurl);
			}
			else
				emit newUrl(tr("Can't parse URL (Reply URL: <a href=\"%1\">%1</a>)").arg(reply->url().toString()));

		}
	}
	emit requestFinished();
	deleteLater();
}

void Uploader::newRequest(const QNetworkReply *const old, const QString &link)
{
	if(!old || link.isEmpty())
		return;

	QUrl netrequrl(link);
	if (netrequrl.host().isEmpty())
		netrequrl = QUrl("http://" + old->url().encodedHost() + link);
	QNetworkRequest netreq(netrequrl);

	QNetworkReply* reply = manager_->get(netreq);
	connect(reply, SIGNAL(uploadProgress(qint64 , qint64)), SIGNAL(uploadProgress(qint64 , qint64)));
}
