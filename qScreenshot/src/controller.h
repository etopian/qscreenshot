/*
 * controller.h
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


#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QSystemTrayIcon>

class Screenshot;
class QMenu;

class Controller : public QObject
{
	Q_OBJECT
public:
	Controller();
	~Controller();

private slots:
	void trayActivated(QSystemTrayIcon::ActivationReason);

private:
	void doUpdate();
	void buildTray();

	Screenshot* screenshot;
	QMenu* trayMenu_;
};

#endif // CONTROLLER_H
