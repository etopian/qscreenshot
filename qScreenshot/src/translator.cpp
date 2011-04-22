/*
 * translator.cpp
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
#include <QLocale>
#include "translator.h"
#include "options.h"
#include "defines.h"

Translator* Translator::instance_ = 0;

Translator* Translator::instance()
{
	if(!instance_) {
		instance_ = new Translator();
	}

	return instance_;
}

Translator::Translator()
	: QTranslator(qApp)
{
	QVariant vCur = Options::instance()->getOption(constCurLang);
	if(vCur == QVariant::Invalid) {
		if(QLocale::system().name().contains("ru", Qt::CaseInsensitive)) {
			vCur = "ru.qm";
		}
	}
	retranslate(vCur.toString());
}

Translator::~Translator()
{
	qApp->removeTranslator(this);
}

void Translator::reset()
{
	delete instance_;
	instance_ = 0;
}

void Translator::retranslate(const QString& fileName)
{
	if(load(fileName, ":/lang/lang/")) {
		qApp->installTranslator(this);
	}
	else {
		qApp->removeTranslator(this);
	}
	Options::instance()->setOption(constCurLang, fileName);
}
