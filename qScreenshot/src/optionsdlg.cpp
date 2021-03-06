/*
 * optionsdlg.cpp
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

#include "optionsdlg.h"
#include "ui_optionsdlg.h"

OptionsDlg::OptionsDlg(QWidget* p)
	: QDialog(p)
	, ui_(new Ui::OptionsDlg)
{
	ui_->setupUi(this);
	ui_->options->restoreOptions();
}

OptionsDlg::~OptionsDlg()
{
	delete ui_;
}

void OptionsDlg::accept()
{
	ui_->options->applyOptions();
	QDialog::accept();
}
