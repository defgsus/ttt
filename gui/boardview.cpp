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
#include "messagebox.h"

#include "engine/board.h"

#include <QResizeEvent>
#include <QRect>
#include <QColor>
#include <QPainter>
#include <QMessageBox>

BoardView::BoardView(QWidget *parent)
    : QWidget       (parent),
      board_        (),
      size_         (board_.size()),
      hoverSquare_  (-1),
      showMessage_  (-1)
{
    setMouseTracking(true);

    updateTimer_.setSingleShot(true);
    connect(&updateTimer_, SIGNAL(timeout()), SLOT(update()));

    // config
    background_ = QBrush(QColor(0,0,0));
    bBrush_ = QBrush(QColor(30,30,30));
    bBrushH_ = QBrush(QColor(40,50,40));
    xPen_ = QPen(QColor(220,255,220));
    oPen_ = QPen(QColor(220,220,255));
    cPen_ = QPen(QColor(100,100,100));
    mPen_ = QPen(QColor(250,250,250));
    setBoard(board_);
}

BoardView::~BoardView()
{
    //delete board_;
}

void BoardView::setBoard(const TTT::Board& b)
{
    size_ = b.size();
    board_ = b;

    if (boardp_.size() != size_*size_)
        boardp_.resize(size_*size_);

    // copy board to paint board
    for (size_t i=0; i<size_*size_; ++i)
    {
        PaintWhat w = Nothing;
        int c = board_.capturedAt(i);
        if (c>1)
            w = Locked2;
        else if (c)
            w = Locked1;
        else
        {
            c = board_.pieceAt(i);
            if (c == TTT::X)
                w = PlayerX;
            else if (c == TTT::O)
                w = PlayerO;
        }

        boardp_[i].what = w;
    }

    update();
}



void BoardView::message(const QString& m)
{
    message_ = m;
    showMessage_ = 0;
    //new MessageBox(m, this);
    //update();
    /*
    QMessageBox::information(this,
                             "X in a row in N²",
                             m);
    */
}

void BoardView::updateIn(int ms)
{
    updateTimer_.start(ms);
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

    mFont_.setPointSizeF(0.5 * sqs_);
}

void BoardView::paintEvent(QPaintEvent * )
{
    // full background
    QPainter p(this);
    p.setFont(mFont_);


    p.setPen(Qt::NoPen);
    p.setBrush(background_);
    p.drawRect(rect());

    for (size_t i = 0; i < boardp_.size(); ++i)
    {
        const int wi = xPen_.width(), wi2 = sqs_/4;
        const QRect
            r0 = squareRect(i),
            r = QRect(r0.left()+wi, r0.top()+wi, r0.width()-wi*2, r0.height()-wi*2),
            r1 = QRect(r0.left()+wi2, r0.top()+wi2, r0.width()-wi2*2, r0.height()-wi2*2);

        // draw background square
        p.setPen(Qt::NoPen);
        p.setBrush((int)i == hoverSquare_ ? bBrushH_ : bBrush_);
        p.drawRect(r0);

        // draw text message
        if (showMessage_>=0)
        {
            if ((int)i < showMessage_ && (int)i < message_.size())
            {
                p.setPen(mPen_);
                p.drawText(r, Qt::AlignCenter | Qt::AlignHCenter, message_.at(i));
            }
            continue;
        }

        // draw piece
        p.setBrush(Qt::NoBrush);
        switch (boardp_[i].what)
        {
        default: break;
        case PlayerX:
            p.setPen(xPen_);
            p.drawLine(r.left(), r.top(), r.right(), r.bottom());
            p.drawLine(r.left(), r.bottom(), r.right(), r.top());
        break;
        case PlayerO:
            p.setPen(oPen_);
            p.drawEllipse(r);
        break;
        case Locked2:
            p.setPen(cPen_);
            p.drawLine(r1.left(), r1.top(), r1.right(), r1.bottom());
            p.drawLine(r1.left(), r1.bottom(), r1.right(), r1.top());
        break;
        case Locked1:
            p.setPen(cPen_);
            p.drawEllipse(r1);
        break;
        }
    }

    if (showMessage_>=0 && showMessage_ < message_.size())
    {
        showMessage_++;
        updateIn(200);
    }

//    QWidget::paintEvent(e);
}

void BoardView::leaveEvent(QEvent *)
{
    if (hoverSquare_)
    {
        hoverSquare_ = TTT::InvalidMove;
        update();
    }
}

void BoardView::mouseMoveEvent(QMouseEvent * e)
{
    TTT::Square s = squareAt(e->x(), e->y());

    int oldHoverSquare_ = hoverSquare_;

    // on hover
    if (!e->buttons())
    {
        if (canMoveTo(s))
        {
            // set highlight
            hoverSquare_ = s;
        }
        else
            hoverSquare_ = TTT::InvalidMove;
    }

    if (hoverSquare_ != oldHoverSquare_)
        update();
}


void BoardView::mousePressEvent(QMouseEvent * e)
{
    TTT::Square s = squareAt(e->x(), e->y());

    // click
    if (e->button() == Qt::LeftButton)
    {
        if (canMoveTo(s))
        {
            hoverSquare_ = TTT::InvalidMove;
            e->accept();
            emit moveMade(s);
            update();
            return;
        }
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

TTT::Square BoardView::squareAt(int x, int y) const
{
    const int
        sx = (x - xo_) / sqs_,
        sy = (y - yo_) / sqs_;

    return (sx >= 0 && sx < (int)size_ &&
            sy >= 0 && sy < (int)size_)?
                sy * size_ + sx : TTT::InvalidMove;

}

bool BoardView::canMoveTo(TTT::Square s) const
{
    const TTT::Stm stm = board_.stm();

    if (s == TTT::InvalidMove || s >= board_.size() * board_.size())
        return false;

    return board_.canMoveTo(stm, s);
}
