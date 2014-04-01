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

#include <QDebug>
#include <QKeyEvent>
#include <QLayout>

#include "mainwidget.h"
#include "engine.h"

#include "popwidget.h"
#include "boardview.h"


MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    board_      (5,4),
    helper_     (board_),
    engine_     (new Engine),
    playerStm_  (TTT::X),
    engineStm_  (TTT::O)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0,0,0));
    setPalette(pal);

    QLayout * l = new QVBoxLayout(this);
    l->setMargin(0);

        boardView_ = new BoardView(this);

        PopWidget * p = new PopWidget(boardView_, this);
        l->addWidget(p);

}



void MainWidget::slotStart()
{
    board_.init();
    if (board_.stm() != playerStm_)
        board_.flipStm();

    boardView_->setBoard(board_);
}

void MainWidget::slotMoveMade(TTT::Move s)
{
    if (s == TTT::InvalidMove)
    {
        // means engine gives up (That's an error actually!)
        if (board_.stm() == engineStm_)
        {
            boardView_->message("I'm lost!");
        }
        return;
    }

    // apply move
    board_.makeMove(s);
    board_.flipStm();

    boardView_->setBoard(board_);

    // check result
    QString m;
    if (board_.isDraw())
    {
        m = tr("Draw!");
    }
    else
    if (helper_.isWin(board_, playerStm_))
    {
        m = tr("You win!");
    }
    else
    if (helper_.isWin(board_, engineStm_))
    {
        m = tr("You lose!");
    }

    if (!m.isNull())
    {
        boardView_->message(m);
        //slotStart();
        return;
    }

    // engine's turn?
    if (board_.stm() == engineStm_)
    {
        engine_->setBoard(board_);
    }
}

