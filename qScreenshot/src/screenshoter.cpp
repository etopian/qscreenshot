#include "screenshoter.h"
#include "grabareawidget.h"
#include "qxtwindowsystem.h"

#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>

Screenshoter::Screenshoter(QObject *parent)
	: QObject(parent)
	, grabAreaWidget_(0)
{
}

Screenshoter::~Screenshoter()
{
	delete grabAreaWidget_;
}

void Screenshoter::captureArea(int delay)
{
	grabAreaWidget_ = new GrabAreaWidget();
	if(grabAreaWidget_->exec() == QDialog::Accepted) {
		QTimer::singleShot(delay*1000, this, SLOT(shootArea()));
	}
	else {
		delete grabAreaWidget_;
		grabAreaWidget_ = 0;
		qApp->desktop()->repaint();
		emit refreshWindow();
	}
}

void Screenshoter::shootArea()
{
	if(!grabAreaWidget_) {
		return;
	}
	QPixmap pix;
	const QRect rect = grabAreaWidget_->getRect();
	if(rect.isValid()) {
		qApp->desktop()->repaint();
		qApp->beep();
		pix = QPixmap::grabWindow(QApplication::desktop()->winId(), rect.x(), rect.y(), rect.width(), rect.height());
	}

	delete grabAreaWidget_;
	grabAreaWidget_ = 0;

	if(!pix.isNull())
		emit pixmapReady(pix);
	else
		emit refreshWindow();
}

void Screenshoter::captureWindow(int delay)
{
	QTimer::singleShot(delay*1000, this, SLOT(shootWindow()));
}

void Screenshoter::shootWindow()
{
	shoot(QxtWindowSystem::activeWindow());
}

void Screenshoter::captureDesktop(int delay)
{
	QTimer::singleShot(delay*1000, this, SLOT(shootDesktop()));
}

void Screenshoter::shootDesktop()
{
	shoot(QApplication::desktop()->winId());
}

void Screenshoter::shoot(WId id)
{
	qApp->beep();
	emit pixmapReady(QPixmap::grabWindow(id));
}
