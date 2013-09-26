/*
 * screenshotmainwin.h
 * Copyright (C) 2009-2011  Khryukin Evgeny
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

#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QMainWindow>
#include <QPointer>

class Server;
class ScreenshotOptions;
class QLabel;
class Screenshoter;
class Uploader;

namespace Ui {
	class Screenshot;
}

class ScreenshotMainWin : public QMainWindow
{
    Q_OBJECT

public:
	ScreenshotMainWin();
	~ScreenshotMainWin();

protected:
	void closeEvent(QCloseEvent *);
	bool eventFilter(QObject *o, QEvent *e);

public slots:
	void captureDesktop();
	void openImage();
	void newScreenshot();
	void action();

	void doOptions();
	void doProxySettings();

signals:
	void screenshotSaved(const QString&);

private slots:
	void saveScreenshot();
	void uploadScreenshot();
	void printScreenshot();
	void cancelUpload();
	void dataTransferProgress( qint64 done, qint64 total );
	void newScreenshotCanceled();
	void pixmapAdjusted();
	void fixSizes();
	void setModified(bool m);
	void aboutQt();
	void doHomePage();
	void doHistory();
	void doAbout();
	void doCheckUpdates();
	void settingsChanged(const QString& option, const QVariant& value);
	void copyUrl();
	void refreshWindow();
	void newPixmapCaptured(const QPixmap& pix);
	void updateUrlLabel(const QString &text);
	void addToHistory(const QString& imageurl);
	void restoreWidgetsState();

private:
	void bringToFront();
	void connectMenu();
	void setServersList(const QStringList& servers_);
	void refreshSettings();
	void saveGeometry();
	void setupStatusBar();
	void updateStatusBar();
	void prepareWidgetsForUpload();
	void autoSaveScreenshot();

private:
	bool modified_;
	QString format_;
	QString fileNameFormat_;
	QString lastFolder_;
	bool autoSave_;
	QString autosaveFolder_;
	QList<Server*> servers_;
	QStringList history_;
	QLabel *sbLbSize_;
	QPointer<ScreenshotOptions> so_;
	Screenshoter* screenshoter_;
	QPointer<Uploader> uploader_;

	Ui::Screenshot *ui_;
};


#endif
