/*
 * iconset.cpp
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

#include "iconset.h"
#include <QFile>
#include <QDebug>
#include <QApplication>

Iconset* Iconset::instance_ = 0;

Iconset* Iconset::instance()
{
	if(!instance_) {
		instance_ = new Iconset();
	}

	return instance_;
}

Iconset::Iconset()
	: QObject(0)
{
}

Iconset::~Iconset()
{
}

void Iconset::reset()
{
	delete instance_;
	instance_ = 0;
}

QIcon Iconset::getIcon(const QString& name)
{
	QIcon ico;
	const QString file = QString(":/icons/icons/%1.png").arg(name);
	QFile f(file);
	if(f.open(QIODevice::ReadOnly)) {
		ico = QIcon(file);
	}
	else {
		qDebug() << QString("Invalid icon name %1").arg(file);
	}
	return ico;
}

QIcon Iconset::getIcon(QStyle::StandardPixmap sp)
{
	return qApp->style()->standardIcon(sp);
}
