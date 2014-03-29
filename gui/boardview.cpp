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

#include "boardview.h"

#include <QResizeEvent>
#include <QRect>
#include <QColor>
#include <QPainter>


BoardView::BoardView(QWidget *parent)
    : QWidget   (parent),
      size_     (3)
      //board_    (new TTT::Board)
{
    background_ = QBrush(QColor(0,0,0));
    xPen_ = QPen(QColor(220,255,220));
    oPen_ = QPen(QColor(220,220,255));
    cPen_ = QPen(QColor(100,100,100));
}

BoardView::~BoardView()
{
    //delete board_;
}

void BoardView::setBoard(const TTT::Board& b)
{
    size_ = b.size();
    //board_ = b;
}



// ----------------- events ---------------------

void BoardView::resizeEvent(QResizeEvent *e)
{
    const int
        mi = std::min(e->size().width(), e->size().height());

    // edge margin
    margin_ = mi / 20;

    const int mi2 = mi - margin_*2;

    // square size
    sqs_ = mi2 / size_;
    // distance from squares to each other
    sqmargin_ = sqs_ / 5;
    // screen offset
    xo_ = (e->size().width() - mi2) / 2;
    yo_ = (e->size().height() - mi2) / 2;

    // pen widths
    xPen_.setWidth(sqs_ / 10);
    oPen_.setWidth(xPen_.width());
    cPen_.setWidth(xPen_.width());
}

void BoardView::paintEvent(QPaintEvent * )
{
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(background_);
    p.drawRect(rect());

    p.setBrush(Qt::NoBrush);
    for (size_t i = 0; i < size_*size_; ++i)
    {
        const QRect r = squareRect(i);


        if (true)//board_.pieceAt(i) == TTT::X)
        {
            p.setPen(xPen_);
            p.drawLine(r.left(), r.top(), r.right(), r.bottom());
            p.drawLine(r.left(), r.bottom(), r.right(), r.top());
        }
        /*
        else
        if (board_.pieceAt(i) == TTT::O)
        {
            p.setPen(oPen_);
            p.drawEllipse(r);
        }*/
    }
}


// -------------------- private stuff ------------

QRect BoardView::squareRect(TTT::Square sq) const
{
    const int
        s = size_,
        x = sq % s,
        y = sq / s;

    return QRect(xo_ + x * sqs_ + sqmargin_/2,
                 yo_ + y * sqs_ + sqmargin_/2,
                 sqs_ - sqmargin_, sqs_ - sqmargin_);
}
