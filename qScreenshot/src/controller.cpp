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

#include "controller.h"
#include "screenshot.h"
#include "server.h"
#include "options.h"
#include "iconset.h"
#include "defines.h"
#include "shortcutmanager.h"
#include "translator.h"


//static const QString imageShack = "ImageShack.us&split&http://load.imageshack.us/&split&&split&&split&uploadtype=image&split&fileupload&split&(?:<div id=\"safari\" class=\"listbox\">.*)(?:<label><a href=\")(.*imageshack.*)(?:\"\\sonClick.*>.*</a></label>)&split&true";
static const QString radikal = "Radikal.ru&split&http://www.radikal.ru/action.aspx&split&&split&&split&upload=yes&split&F&split&<input\\s+id=\"input_link_1\"\\s+value=\"([^\"]+)\"&split&true";
static const QString pixacadem = "Pix.Academ.org&split&http://pix.academ.org/&split&&split&&split&action=upload_image&split&image&split&<div id='link'><a id=\"original\" href=\"(http[^\"]+)\"&split&true";
static const QString kachalka = "Kachalka.com&split&http://www.kachalka.com/upload.php&split&&split&&split&&split&userfile[]&split&name=\"option\" value=\"(http://www.kachalka.com/[^\"]+)\" /></td>&split&true";

static const QStringList staticHostsList = QStringList() //<< imageShack
					<< pixacadem << radikal << kachalka;


static bool isListContainsServer(const QString& server, const QStringList& servers)
{
	foreach(QString serv, servers) {
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
	Options* o = Options::instance();
	QVariant vServers = o->getOption(constServerList);

	if(!vServers.isValid()) { //приложение запущено впервые
		o->setOption(constShortCut, QVariant("Ctrl+Shift+S"));
		o->setOption(constFormat, QVariant("png"));
		o->setOption(constFileName, QVariant("pic-yyyyMMdd-hhmmss"));
		o->setOption(constDelay, QVariant(0));
		o->setOption(constVersionOption, APP_VERSION);
	}

	QStringList servers = vServers.toStringList();
	foreach(const QString& host, staticHostsList) {
		if(!isListContainsServer(host, servers))
			servers.append(host);
	}

	if(o->getOption(constVersionOption).toString() != APP_VERSION) {
		foreach(const QString& host, staticHostsList) {
			updateServer(&servers, host);
		}

		doUpdate();
		o->setOption(constVersionOption, APP_VERSION);
	}

	o->setOption(constServerList, servers); //сохраняем обновленный список серверов

	if(!ShortcutManager::instance()->setShortcut(QKeySequence(o->getOption(constShortCut).toString()))) {
		QMessageBox::critical(0, tr("Error"), tr("Failed to register shortcut!"), QMessageBox::Ok);
	}
	
	screenshot = new Screenshot();
	connect(ShortcutManager::instance(), SIGNAL(activated()), screenshot, SLOT(shootScreen()));

	buildTray();
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
	QSystemTrayIcon *tray = new QSystemTrayIcon(this);
	tray->setIcon(Iconset::instance()->getIcon("screenshot"));
	tray->setToolTip(QString(APP_NAME) + " " + QString(APP_VERSION));
	connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	trayMenu_ = new QMenu();
	trayMenu_->addAction(tr("Grab screen"), screenshot, SLOT(shootScreen()));
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

	tray->setContextMenu(trayMenu_);
	tray->show();
}

void Controller::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::DoubleClick) {
		screenshot->shootScreen();
	}
}

void Controller::doUpdate()
{
	// do some updates
}

void Controller::retranslate(const QString &trans)
{
	Translator::instance()->retranslate(trans);
	QMessageBox::information(0, tr("Translate"), tr("Restart application!"), QMessageBox::Ok);
}

#include "controller.moc"
