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

#include <QPainter>

#include "squarewidget.h"

SquareWidget::SquareWidget(QWidget *parent)
    :   QWidget (parent),
        hover_  (0)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    br_back_ =  QBrush(QColor(30,30,30));
    br_hover_ = QBrush(QColor(40,50,40));
    pen_select_ = QPen(QColor(255,255,255));
}



void SquareWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // draw square
    p.setPen(Qt::NoPen);
    p.setBrush(hover_? br_hover_ : br_back_);
    p.drawRect(rect());

    // draw cross
    if (selected_)
    {
        const int wi = width() / 10;
        const QRect
            r = QRect(rect().left()+wi, rect().top()+wi,
                      rect().width()-wi*2, rect().height()-wi*2);

        p.setBrush(Qt::NoBrush);
        pen_select_.setWidth(wi);
        p.setPen(pen_select_);

        p.drawLine(r.left(), r.top(), r.right(), r.bottom());
        p.drawLine(r.left(), r.bottom(), r.right(), r.top());
    }
}

void SquareWidget::enterEvent(QEvent *)
{
    hover_ = true;
    update();
}

void SquareWidget::leaveEvent(QEvent *)
{
    hover_ = false;
    update();
}

void SquareWidget::mousePressEvent(QMouseEvent * )
{
    selected_ = !selected_;
    update();

    emit clicked();
}

void SquareWidget::resizeEvent(QResizeEvent * )
{
    if (height() != width())
    {
        setFixedHeight(width());
    }

}
