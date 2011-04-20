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

#include "controller.h"
#include "screenshot.h"
#include "server.h"
#include "options.h"
#include "iconset.h"
#include "defines.h"
#include "shortcutmanager.h"
#include <QMessageBox>

const QString imageShack = "ImageShack.us&split&http://load.imageshack.us/&split&&split&&split&uploadtype=image&split&fileupload&split&(?:<div id=\"safari\" class=\"listbox\">.*)(?:<label><a href=\")(.*imageshack.*)(?:\"\\sonClick.*>.*</a></label>)&split&true";
const QString radikal = "Radikal.ru&split&http://www.radikal.ru/action.aspx&split&&split&&split&upload=yes&split&F&split&<input\\s+id=\"input_link_1\"\\s+value=\"([^\"]+)\"&split&true";
const QString pixacadem = "Pix.Academ.org&split&http://pix.academ.org/&split&&split&&split&action=upload_image&split&image&split&<div id='link'><a id=\"original\" href=\"(http[^\"]+)\"&split&true";
const QString kachalka = "Kachalka.com&split&http://www.kachalka.com/upload.php&split&&split&&split&&split&userfile[]&split&name=\"option\" value=\"(http://www.kachalka.com/[^\"]+)\" /></td>&split&true";


Controller::Controller()
	: QObject()
	, screenshot(0)
	, trayMenu_(0)
{
	Options* o = Options::instance();
	QVariant vServers = o->getOption(constServerList);

	if(!vServers.isValid()) { //приложение запущено впервые
		o->setOption(constShortCut, QVariant("Alt+Shift+S"));
		o->setOption(constFormat, QVariant("png"));
		o->setOption(constFileName, QVariant("pic-yyyyMMdd-hhmmss"));
		o->setOption(constDelay, QVariant(0));
	}

	QStringList servers = vServers.toStringList();
	QStringList hostsList = QStringList() << pixacadem << imageShack
					 << radikal << kachalka;
	foreach(const QString& host, hostsList) {
		if(!isListContainsServer(host, servers))
			servers.append(host);
	}
	o->setOption(constServerList, servers); //сохраняем обновленный список серверов

	if(!ShortcutManager::instance()->setShortcut(QKeySequence(o->getOption(constShortCut).toString()))) {
		QMessageBox::critical(0, tr("Error"), tr("Failed to register shortcut!"), QMessageBox::Ok);
	}

	buildTray();
	
	screenshot = new Screenshot();
	connect(ShortcutManager::instance(), SIGNAL(activated()), screenshot, SLOT(shootScreen()));
}

Controller::~Controller()
{
	delete screenshot;
	delete trayMenu_;
	Options::reset();
	Iconset::reset();
	ShortcutManager::reset();
}

void Controller::onShortCutActivated()
{
	screenshot->shootScreen();
}

void Controller::newScreenshot()
{
	screenshot->newScreenshot();
}

void Controller::openImage()
{
	screenshot->openImage();
}

void Controller::buildTray()
{
	QSystemTrayIcon *tray = new QSystemTrayIcon(this);
	tray->setIcon(Iconset::instance()->getIcon("screenshot"));
	connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	trayMenu_ = new QMenu();
	trayMenu_->addAction(tr("Grab screen"), this, SLOT(onShortCutActivated()));
	trayMenu_->addAction(tr("New Screenshot"), this, SLOT(newScreenshot()));
	trayMenu_->addAction(tr("Open Image"), this, SLOT(openImage()));
	trayMenu_->addSeparator();
	trayMenu_->addAction(tr("Exit"), qApp, SLOT(quit()));

	tray->setContextMenu(trayMenu_);
	tray->show();
}

void Controller::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::DoubleClick) {
		onShortCutActivated();
	}
}

bool Controller::isListContainsServer(const QString& server, const QStringList& servers)
{
	foreach(QString serv, servers) {
		if(serv.split(Server::splitString()).first() == server.split(Server::splitString()).first())
			return true;
	}
	return false;
}
