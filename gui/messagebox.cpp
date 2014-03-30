/***************************************************************************

Copyright (C) 2014  stefan.berke @ modular-audio-graphics.com

This source is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

****************************************************************************/

#include "messagebox.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

MessageBox::MessageBox(const QString& m, QWidget *parent)
    :   QWidget (parent),
        message_(m)
{
    Q_ASSERT(parent);

    QSize s = parent->size()/2;
    setGeometry((parent->width()-s.width())/2,
                (parent->height()-s.height())/2,
                s.width(), s.height());

    bBrush_ = QBrush(QColor(100,100,100,100));
}


void MessageBox::mousePressedEvent(QMouseEvent * e)
{
    e->accept();
    deleteLater();
}

void MessageBox::paintEvent(QPaintEvent * )
{
    QPainter p(this);

    p.drawRect(rect());

    p.setBrush(QBrush(QColor(0,255,0)));
    p.drawText(rect(),
               Qt::AlignCenter | Qt::AlignHCenter,
               message_);
}
