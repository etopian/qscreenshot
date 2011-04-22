/*
 * aboutdlg.cpp
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

#include "aboutdlg.h"
#include "ui_aboutdlg.h"
#include "defines.h"
#include "iconset.h"

AboutDlg::AboutDlg(QWidget *p)
	: QDialog(p)
	, ui_(new Ui::AboutDlg)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui_->setupUi(this);

	const QPixmap pix = Iconset::instance()->getIcon("screenshot").pixmap(48);
	ui_->lbl_icon->setPixmap(pix);

	ui_->lbl_app_name->setText(QString(APP_NAME) + " " + QString(APP_VERSION));
	show();
}

AboutDlg::~AboutDlg()
{
	delete ui_;
}
