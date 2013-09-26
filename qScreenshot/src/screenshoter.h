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

#ifndef SCREENSHOTER_H
#define SCREENSHOTER_H

#include <QPixmap>

class GrabAreaWidget;

class Screenshoter : public QObject
{
	Q_OBJECT
public:
	explicit Screenshoter(QObject *parent = 0);
	~Screenshoter();
	
signals:
	void refreshWindow();
	void pixmapReady(const QPixmap&);
	
public slots:
	void captureDesktop(int delay = 0);
	void captureWindow(int delay = 0);
	void captureArea(int delay = 0);

private slots:
	void shootDesktop();
	void shootWindow();
	void shootArea();

private:
	void shoot(WId id);

private:
	GrabAreaWidget* grabAreaWidget_;
};

#endif // SCREENSHOTER_H
