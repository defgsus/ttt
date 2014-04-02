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
#include "settings.h"
#include "engine.h"

#include "popgroup.h"
#include "popbutton.h"
#include "boardview.h"
#include "settingswidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    board_      (5,4),
    helper_     (board_),
    engine_     (new Engine),
    ignoreEngine_(false),
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

    PopGroup * pg = new PopGroup(this);
    l->addWidget(pg);

        PopWidget * boardDock = new PopWidget(this);
        //l->addWidget(boardDock);

        QLayout * l2 = new QHBoxLayout(boardDock);
        l2->setMargin(0);

            b_back_ = new PopButton(PopButton::Left, boardDock);
            l2->addWidget(b_back_);
            connect(b_back_, SIGNAL(clicked()), SLOT(back()));

            boardView_ = new BoardView(boardDock);
            l2->addWidget(boardView_);

            b_fwd_ = new PopButton(PopButton::Right, boardDock);
            l2->addWidget(b_fwd_);
            connect(b_fwd_, SIGNAL(clicked()), SLOT(forward()));


        SettingsWidget * settings_ = new SettingsWidget(this);
        //l->addWidget(settings_);

    pg->addWidget(boardDock);
    pg->addWidget(settings_);


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
    updateStackButtons_();

    board_.init();

    if (board_.stm() != playerStm_)
        board_.flipStm();

    boardView_->setBoard(board_);
}

void MainWidget::slotReconfigure()
{
    uint x = AppSettings->getValue("consecutives").toInt(),
        n = AppSettings->getValue("boardSize").toInt();
    if (board_.size() != n || board_.consecutives() != x)
    {
        ignoreEngine();
        board_.setSize(n, x);
        boardView_->setBoard(board_);
    }

    engine_->setMaxDepth(AppSettings->getValue("maxDepth").toInt());
}

void MainWidget::ignoreEngine()
{
    if (!engine_->thinking()) return;

    ignoreEngine_ = true;
    engine_->stop();
}

void MainWidget::slotMoveMade(TTT::Move s)
{
    if (ignoreEngine_ && board_.stm() == engineStm_)
    {
        ignoreEngine_ = false;
        return;
    }

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

    // flip side-to-move
    board_.flipStm();

    // update view
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
    b_fwd_->setEnabled(stack_pos_ + 1 < stack_.size());
    b_back_->setEnabled(stack_pos_ > 0);
}

void MainWidget::push()
{
    stack_.resize(stack_pos_+1);
    stack_[stack_pos_] = board_;
    stack_pos_++;

    //qDebug() << "push " << stack_.size() << "p" << stack_pos_;

    updateStackButtons_();
}

void MainWidget::back()
{
    // save current board if it hasn't been done
    if (board_.stm() == playerStm_
        && stack_pos_ == stack_.size())
    {
        push();
        stack_pos_--;
    }

    // go back in time
    if (stack_pos_ > 0)
    {
        stack_pos_--;

        board_ = stack_[stack_pos_];
        boardView_->setBoard(board_);
    }

    //qDebug() << "back " << stack_.size() << "p" << stack_pos_;

    updateStackButtons_();
}

void MainWidget::forward()
{
    // go forward in time
    if (stack_pos_ + 1 < stack_.size())
    {
        stack_pos_++;

        board_ = stack_[stack_pos_];
        boardView_->setBoard(board_);

    }

    //qDebug() << "fwd " << stack_.size() << "p" << stack_pos_;

    updateStackButtons_();
}
