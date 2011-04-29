/*
 * editserverdlg.cpp
 * Copyright (C) 2009-2011  Khryukin Evgeny
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

#include "editserverdlg.h"
#include "server.h"
#include "ui_editserverdlg.h"


EditServerDlg::EditServerDlg(QWidget *parent)
	: QDialog(parent)
	, ui_(new Ui::EditServerDlg)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setModal(false);
	ui_->setupUi(this);

	connect(ui_->buttonBox, SIGNAL(accepted()), SLOT(onOkPressed()));
}

EditServerDlg::~EditServerDlg()
{
	delete ui_;
}

void EditServerDlg::onOkPressed()
{
	QStringList l = QStringList() << ui_->le_name->text() << ui_->le_url->text() << ui_->le_user->text() << ui_->le_pass->text();
	l <<  ui_->le_post_data->text() << ui_->le_file_input->text() << ui_->le_regexp->text();
	l << (ui_->cb_proxy->isChecked() ? "true" : "false");
	const QString str = l.join(Server::splitString());
	if(server_) {
		server_->setFromString(str);
		server_->setText(server_->displayName());
	}
	emit okPressed(str);
	close();
}

void EditServerDlg::setSettings(const QString& settings)
{
		QStringList l = settings.split(Server::splitString());

		if(!l.isEmpty())
			ui_->le_name->setText(l.takeFirst());
		if(!l.isEmpty())
			ui_->le_url->setText(l.takeFirst());
		if(!l.isEmpty())
			ui_->le_user->setText(l.takeFirst());
		if(!l.isEmpty())
			ui_->le_pass->setText(l.takeFirst());
		if(!l.isEmpty())
			ui_->le_post_data->setText(l.takeFirst());
		if(!l.isEmpty())
			ui_->le_file_input->setText(l.takeFirst());
		if(!l.isEmpty())
			ui_->le_regexp->setText(l.takeFirst());
		if(!l.isEmpty())
			ui_->cb_proxy->setChecked(l.takeFirst() == "true");
}

void EditServerDlg::setServer(Server *const s)
{
	server_ = s;
	setSettings(s->settingsToString());
}
