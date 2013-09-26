/*
 * proxysettingsdlg.cpp
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


#include "proxysettingsdlg.h"
#include "options.h"
#include "defines.h"
#include "ui_proxysettingsdlg.h"

ProxySettingsDlg::ProxySettingsDlg(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ProxySettingsDlg)
{
	ui->setupUi(this);
	ui->cb_type->addItems(QStringList() << "HTTP" << "SOCKS5");
	ui->cb_type->setCurrentIndex(0);
	setProxySettings();
}

ProxySettingsDlg::~ProxySettingsDlg()
{
	delete ui;
}

void ProxySettingsDlg::setProxySettings()
{

	Options *o = Options::instance();
	ui->le_host->setText(o->getOption(constProxyHost).toString());
	ui->le_pass->setText(o->getOption(constProxyPass).toString());
	ui->le_port->setText(QString::number(o->getOption(constProxyPort).toInt()));
	ui->le_user->setText(o->getOption(constProxyUser).toString());
	if(o->getOption(constProxyType).toString() == "socks") {
		ui->cb_type->setCurrentIndex(1);
	}
}

void ProxySettingsDlg::accept()
{
	Options *o = Options::instance();
	if(ui->cb_type->currentText() == "HTTP") {
		o->setOption(constProxyType, "http");
	}
	else {
		o->setOption(constProxyType, "socks");
	}

	o->setOption(constProxyHost, ui->le_host->text());
	o->setOption(constProxyPort, ui->le_port->text().toInt());
	o->setOption(constProxyUser, ui->le_user->text());
	o->setOption(constProxyPass, ui->le_pass->text());

	QDialog::accept();
}
