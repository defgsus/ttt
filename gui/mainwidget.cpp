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
#include <QDockWidget>

#include "mainwidget.h"
#include "engine.h"

#include "popgroup.h"
#include "popbutton.h"
#include "boardview.h"


MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    board_      (5,4),
    helper_     (board_),
    engine_     (new Engine),
    playerStm_  (TTT::X),
    engineStm_  (TTT::O)
{
    // -- style --

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0,0,0));
    setPalette(pal);


    // -- widgets --

    QVBoxLayout * l = new QVBoxLayout(this);
    l->setMargin(0);

    //PopGroup * pg = new PopGroup(this);
    //l->addWidget(pg);

        QLayout * l2 = new QHBoxLayout;
        l->addLayout(l2);

            b_back_ = new PopButton(PopButton::Left, this);
            l2->addWidget(b_back_);
            connect(b_back_, SIGNAL(clicked()), SLOT(back()));

            boardView_ = new BoardView(this);
            l2->addWidget(boardView_);

            b_fwd_ = new PopButton(PopButton::Right, this);
            l2->addWidget(b_fwd_);
            connect(b_fwd_, SIGNAL(clicked()), SLOT(forward()));

    //pg->addWidget(boardView_);
    //pg->addWidget(new BoardView(this));


    // --- state logic ---

    // connect move makers
    connect(boardView_, SIGNAL(moveMade(TTT::Move)), SLOT(slotMoveMade(TTT::Move)));
    connect(engine_, SIGNAL(moveMade(TTT::Move)), SLOT(slotMoveMade(TTT::Move)));
    connect(boardView_, SIGNAL(messageAccepted()), SLOT(slotStart()));

    slotStart();
}

void MainWidget::slotStart()
{
    stack_.clear();
    stack_pos_ = 0;

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

    // store in history
    if (board_.stm() == playerStm_)
        push();

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

void MainWidget::updateStackButtons_()
{
    b_fwd_->setVisible(stack_pos_ < stack_.size());
    b_back_->setVisible(stack_pos_ > 0);
}

void MainWidget::push()
{
    stack_.resize(stack_pos_+1);
    stack_[stack_pos_] = board_;
    stack_pos_++;

    updateStackButtons_();
}

void MainWidget::back()
{
    if (stack_pos_ > 0)
    {
        stack_pos_--;

        board_ = stack_[stack_pos_];
        boardView_->setBoard(board_);
    }

    updateStackButtons_();
}

void MainWidget::forward()
{
    if (stack_pos_ < stack_.size())
    {
        board_ = stack_[stack_pos_];
        boardView_->setBoard(board_);

        stack_pos_++;
    }

    updateStackButtons_();
}
