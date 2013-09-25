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

#include "grabareawidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>


GrabAreaWidget::GrabAreaWidget()
	: QDialog()
	, startPoint(QPoint(-1, -1))
	, endPoint(QPoint(-1, -1))
{
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setWindowTitle(tr("Select area"));
	setWindowState(Qt::WindowFullScreen);
	setCursor(Qt::CrossCursor);
	resize(QApplication::desktop()->size());
}

GrabAreaWidget::~GrabAreaWidget()
{
}

QRect GrabAreaWidget::getRect() const
{
	QRect r;
	if(endPoint.x() != -1) {
		r = QRect(qMin(startPoint.x(), endPoint.x()), qMin(startPoint.y(), endPoint.y()),
			  qAbs(startPoint.x() - endPoint.x()), qAbs(startPoint.y() - endPoint.y()));
	}
	return r;
}

void GrabAreaWidget::mousePressEvent(QMouseEvent *e)
{
	if(e->button() == Qt::LeftButton) {
		startPoint = e->pos();
	}
	else {
		QDialog::reject();
	}
}

void GrabAreaWidget::mouseMoveEvent(QMouseEvent *e)
{
	if(e->buttons() & Qt::LeftButton) {
		endPoint = e->pos();
		update();
	}

}

void GrabAreaWidget::mouseReleaseEvent(QMouseEvent *e)
{
	if(!(e->buttons() & Qt::LeftButton)) {
		endPoint = e->pos();
		QDialog::accept();
	}
}

void GrabAreaWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	QColor c("#f0f0f0");
	c.setAlpha(80);
	QRect r = getRect();
	if(r.isValid()) {
		QPainterPath path;
		path.addRect(0, 0, width(), r.top());
		path.addRect(r.right(), r.top(), rect().width() - r.right(), r.height()-1);
		path.addRect(0, r.bottom(), width(), height() - r.bottom());
		path.addRect(0, r.top(), r.left(), r.height()-1);
		painter.fillPath(path, c);

		QPen pen(Qt::gray);
		pen.setWidth(1);
		painter.setPen(pen);
		painter.drawRect(r);
	}
	else {
		painter.fillRect(rect(), c);
	}
}
