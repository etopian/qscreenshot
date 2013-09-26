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
