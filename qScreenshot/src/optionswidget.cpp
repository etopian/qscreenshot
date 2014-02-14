/*
 * optionswidget.cpp
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

#include <QCloseEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QProcess>

#include "optionswidget.h"
#include "editserverdlg.h"
#include "server.h"
#include "options.h"
#include "defines.h"
#include "shortcutmanager.h"
#include "ui_optionswidget.h"


#ifdef Q_WS_WIN
#include <QSettings>

	static const QString regString = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
#endif
#ifdef Q_WS_X11
	static const QString autoStart = "/.config/autostart/qscreenshot.desktop";
	static const QString desktopFile = "[Desktop Entry]\n"
					   "Name=qScreenshot\n"
					   "Encoding=UTF-8\n"
					   "Comment=Create, modify and upload screenshots\n"
					   "Type=Application\n"
					   "Categories=Qt;Graphics;2DGraphics;Viewer;\n"
					   "Exec=qScreenshot\n"
					   "Terminal=false\n"
					   "Icon=screenshot";
#endif


//--------------------------------------------------------
//---GrepShortcutKeyDialog from libpsi with some changes--
//--------------------------------------------------------
class GrepShortcutKeyDialog : public QDialog
{
	Q_OBJECT
public:
	GrepShortcutKeyDialog(QWidget* p = 0)
		: QDialog(p)
		, gotKey(false)
	{
		setAttribute(Qt::WA_DeleteOnClose);
		setModal(true);
		setWindowTitle(tr("New Shortcut"));
		QHBoxLayout *l = new QHBoxLayout(this);
		le = new QLineEdit();
		l->addWidget(le);
		QPushButton *cancelButton = new QPushButton(tr("Cancel"));
		l->addWidget(cancelButton);
		connect(cancelButton, SIGNAL(clicked()), SLOT(close()));

		displayPressedKeys(QKeySequence());

		adjustSize();
		setFixedSize(size());
	}

	void show()
	{
		QDialog::show();
		grabKeyboard();
	}

protected:
	void closeEvent(QCloseEvent *event)
	{
		releaseKeyboard();
		event->accept();
	}

	void keyPressEvent(QKeyEvent* event)
	{
		displayPressedKeys(getKeySequence(event));

		if (!isValid(event->key()) || gotKey)
			return;

		gotKey = true;
		emit newShortcutKey(getKeySequence(event));
		close();
	}

	void keyReleaseEvent(QKeyEvent* event)
	{
		displayPressedKeys(getKeySequence(event));
	}

signals:
	void newShortcutKey(const QKeySequence& key);

private:
	void displayPressedKeys(const QKeySequence& keys)
	{
		QString str = keys.toString(QKeySequence::NativeText);
		if (str.isEmpty())
			str = tr("Set Keys");
		le->setText(str);
	}

	QKeySequence getKeySequence(QKeyEvent* event) const
	{
		return QKeySequence((isValid(event->key()) ? event->key() : 0)
				    + (event->modifiers() & ~Qt::KeypadModifier));
	}

	bool isValid(int key) const
	{
		switch (key) {
		case 0:
		case Qt::Key_unknown:
			return false;
		}

		return !isModifier(key);
	}

	bool isModifier(int key) const
	{
		switch (key) {
		case Qt::Key_Shift:
		case Qt::Key_Control:
		case Qt::Key_Meta:
		case Qt::Key_Alt:
		case Qt::Key_AltGr:
		case Qt::Key_Super_L:
		case Qt::Key_Super_R:
		case Qt::Key_Menu:
			return true;
		}
		return false;
	}

	bool gotKey;
	QLineEdit* le;
};



//---------------------------------------------------
//-------------------OptionsWidget-------------------
//---------------------------------------------------
OptionsWidget::OptionsWidget(QWidget* p)
	: QWidget(p)
	, ui_(new Ui::OptionsWidget)
{
	ui_->setupUi(this);

	Options* o = Options::instance();
	shortCut = o->getOption(constShortCut, QVariant(shortCut)).toString();
	format = o->getOption(constFormat, QVariant(format)).toString();
	fileName = o->getOption(constFileName, QVariant(fileName)).toString();
	servers = o->getOption(constServerList).toStringList();
	defaultAction = o->getOption(constDefaultAction, QVariant(Desktop)).toInt();
	autoSave = o->getOption(constAutosave, false).toBool();
	autosaveFolder = o->getOption(constAutosaveFolder, QDir::homePath()).toString();
	autoCheck = o->getOption(constAutocheckUpdates).toBool();

#ifdef Q_WS_MAC
	ui_->cb_autostart->hide();
#endif

	connect(ui_->pb_add, SIGNAL(clicked()), SLOT(addServer()));
	connect(ui_->pb_del, SIGNAL(clicked()), SLOT(delServer()));
	connect(ui_->pb_edit, SIGNAL(clicked()), SLOT(editServer()));
	connect(ui_->lw_servers, SIGNAL(doubleClicked(QModelIndex)), SLOT(editServer()));
	connect(ui_->pb_modify, SIGNAL(clicked()), SLOT(requstNewShortcut()));
	connect(ui_->tb_autosaveFolder, SIGNAL(clicked()), SLOT(getAutosaveFolder()));
	connect(ui_->pb_defaults, SIGNAL(clicked()), SLOT(defaults()));
}

OptionsWidget::~OptionsWidget()
{
	delete ui_;
}

void OptionsWidget::addServer()
{
	EditServerDlg *esd = new EditServerDlg(this);
	connect(esd, SIGNAL(okPressed(QString)), this, SLOT(addNewServer(QString)));
	esd->show();
}

void OptionsWidget::delServer()
{
	Server *s = (Server*)ui_->lw_servers->currentItem();
	if(!s)
		return;
	ui_->lw_servers->removeItemWidget(s);
	delete(s);
}

void OptionsWidget::editServer()
{
	Server *s = (Server*)ui_->lw_servers->currentItem();
	if(!s)
		return;
	EditServerDlg *esd = new EditServerDlg(this);
	esd->setServer(s);
	esd->show();
}

void OptionsWidget::addNewServer(const QString& settings)
{
	Server *s = new Server(ui_->lw_servers);
	s->setFromString(settings);
	s->setText(s->displayName());
}

void OptionsWidget::applyOptions()
{
	Options* o = Options::instance();

	if(shortCut != ui_->le_shortcut->text()) {
		shortCut = ui_->le_shortcut->text();
		if(!ShortcutManager::instance()->setShortcut(QKeySequence(shortCut))) {
			QMessageBox::critical(0, tr("Error"), tr("Failed to register shortcut!"), QMessageBox::Ok);
		}
		o->setOption(constShortCut, QVariant(shortCut));
	}

	format = ui_->cb_format->currentText();
	o->setOption(constFormat, QVariant(format));

	fileName = ui_->le_filename->text();
	o->setOption(constFileName, QVariant(fileName));

	servers.clear();
	for(int i = 0; i < ui_->lw_servers->count(); i++) {
		Server *s = (Server *)ui_->lw_servers->item(i);
		servers.append(s->settingsToString());
	}
	o->setOption(constServerList, QVariant(servers));

	if(ui_->rb_desktop->isChecked())
		defaultAction = Desktop;
	else if(ui_->rb_area->isChecked())
		defaultAction = Area;
	else
		defaultAction = Window;
	o->setOption(constDefaultAction, defaultAction);

	autosaveFolder = ui_->le_autosaveFolder->text();
	autoSave = ui_->cb_autosave->isChecked();
	o->setOption(constAutosave, autoSave);
	o->setOption(constAutosaveFolder, autosaveFolder);

	autoCheck = ui_->cb_updatesCheck->isChecked();
	o->setOption(constAutocheckUpdates, autoCheck);


#ifdef Q_WS_WIN
	QSettings set(regString, QSettings::NativeFormat);
	if(ui_->cb_autostart->isChecked()) {
		set.setValue(APP_NAME, QDir::toNativeSeparators(qApp->applicationFilePath()));
	}
	else {
		set.remove(APP_NAME);
	}
#endif
#ifdef Q_WS_X11
	QDir home = QDir::home();
	if (!home.exists(".config/autostart")) {
		home.mkpath(".config/autostart");
	}
	QFile f(home.absolutePath() + autoStart);
	if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
		f.write(desktopFile.trimmed().toUtf8());
		f.write(QString("\nHidden=%1").arg(ui_->cb_autostart->isChecked() ? "false\n" : "true\n").toUtf8());
	}
#endif
}

void OptionsWidget::restoreOptions()
{
	QStringList l = QStringList() << "jpg" << "png";
	ui_->cb_format->addItems(l);
	int index = ui_->cb_format->findText(format);
	if(index != -1)
		ui_->cb_format->setCurrentIndex(index);
	ui_->le_filename->setText(fileName);
	ui_->le_shortcut->setText(shortCut);
	foreach(const QString& settings, servers) {
		Server *s = new Server(ui_->lw_servers);
		s->setFromString(settings);
		s->setText(s->displayName());
	}

	ui_->cb_updatesCheck->setChecked(autoCheck);

	ui_->rb_desktop->setChecked(defaultAction == Desktop);
	ui_->rb_area->setChecked(defaultAction == Area);
	ui_->rb_window->setChecked(defaultAction == Window);

	ui_->le_autosaveFolder->setText(autosaveFolder);
	ui_->cb_autosave->setChecked(autoSave);
	ui_->le_autosaveFolder->setEnabled(autoSave);
	ui_->tb_autosaveFolder->setEnabled(autoSave);

#ifdef Q_WS_WIN
	QSettings set(regString, QSettings::NativeFormat);
	const QString path = set.value(APP_NAME).toString();
	ui_->cb_autostart->setChecked( (path == QDir::toNativeSeparators(qApp->applicationFilePath())) );
#endif
#ifdef Q_WS_X11
	QFile desktop(QDir::homePath() + autoStart);
	if (desktop.open(QIODevice::ReadOnly)
		&& QString(desktop.readAll()).contains(QRegExp("\\bhidden\\s*=\\s*false", Qt::CaseInsensitive)))
	{
		ui_->cb_autostart->setChecked(true);
	}
#endif
}

void OptionsWidget::requstNewShortcut()
{
	GrepShortcutKeyDialog *gs = new GrepShortcutKeyDialog(this);
	connect(gs, SIGNAL(newShortcutKey(QKeySequence)), this, SLOT(onNewShortcut(QKeySequence)));
	gs->show();
}

void OptionsWidget::onNewShortcut(const QKeySequence& ks)
{
	ui_->le_shortcut->setText(ks.toString(QKeySequence::NativeText));
}

void OptionsWidget::getAutosaveFolder()
{
	const QString dir = QFileDialog::getExistingDirectory(this, tr("Chose Folder"), autosaveFolder);
	if(!dir.isEmpty()) {
		ui_->le_autosaveFolder->setText(dir);
	}
}

void OptionsWidget::defaults()
{
	int res = QMessageBox::warning(this, tr("Reset Settings"),
				       tr("Are you sure you want to completely\n"
					  "reset settings to the default values?\n"
					  "Application will be restarted."),
				       QMessageBox::Ok, QMessageBox::Cancel);
	if(res == QMessageBox::Ok) {
		connect(qApp, SIGNAL(aboutToQuit()), Options::instance(), SLOT(defaults()));
		qApp->exit(0);
		QProcess::startDetached(QApplication::applicationFilePath());
	}
}

#include "optionswidget.moc"
