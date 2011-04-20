/*
 * main.cpp
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

#include <QApplication>
#include "controller.h"

#define APP_NAME "qScreenshot"
#define APP_VERSION "0.1"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName(APP_NAME);
	app.setApplicationVersion(APP_VERSION);
	app.setOrganizationName(APP_NAME);
	Controller *controller = new Controller();
	int ret = app.exec();
	delete controller;
	return ret;
}
