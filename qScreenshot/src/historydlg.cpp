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

#include "historydlg.h"
#include "iconset.h"

#include <QtGui>

HistoryDlg::HistoryDlg(const QStringList& list, QWidget* p)
	: QDialog(p, Qt::Window)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setModal(false);
	setWindowModality(Qt::NonModal);
	setWindowTitle(tr("History"));
	QVBoxLayout *l = new QVBoxLayout(this);
	lw = new QListWidget(this);
	lw->addItems(list);
	l->addWidget(lw);

	QHBoxLayout *bl = new QHBoxLayout();
	QPushButton *copyButton = new QPushButton(tr("Copy"));
	copyButton->setToolTip(tr("Copy link to the clipboard"));
	copyButton->setIcon(Iconset::instance()->getIcon(QStyle::SP_DialogOpenButton));
	QPushButton *openButton = new QPushButton(tr("Open"));
	openButton->setToolTip(tr("Open link in browser"));
	openButton->setIcon(Iconset::instance()->getIcon(QStyle::SP_BrowserReload));
	QPushButton *closeButton = new QPushButton(tr("Close"));
	closeButton->setToolTip(tr("Close history"));
	closeButton->setIcon(Iconset::instance()->getIcon(QStyle::SP_DialogCloseButton));
	bl->addWidget(copyButton);
	bl->addWidget(openButton);
	bl->addStretch();
	bl->addWidget(closeButton);

	l->addLayout(bl);

	connect(closeButton, SIGNAL(clicked()), SLOT(close()));
	connect(copyButton, SIGNAL(clicked()), SLOT(copy()));
	connect(openButton, SIGNAL(clicked()), SLOT(itemActivated()));
	connect(lw, SIGNAL(activated(QModelIndex)), SLOT(itemActivated()));

	resize(500, 300);
	show();
}

void HistoryDlg::itemActivated()
{
	QListWidgetItem *lwi = lw->currentItem();
	if(lwi) {
		QDesktopServices::openUrl(QUrl(lwi->text()));
	}
}

void HistoryDlg::copy()
{
	QListWidgetItem *lwi = lw->currentItem();
	if(lwi) {
		qApp->clipboard()->setText(lwi->text());
	}
}
