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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "engine.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow (parent),
    ui_         (new Ui::MainWindow),
    board_      (5,4),
    helper_     (board_),
    engine_     (new Engine),
    playerStm_  (TTT::X),
    engineStm_  (TTT::O)
{
    // setup ui
    ui_->setupUi(this);

    // init boardview with correct size
    ui_->boardView->setBoard(board_);

    // connect boardview
    connect(ui_->boardView, SIGNAL(moveMade(TTT::Move)), SLOT(slotMoveMade(TTT::Move)));
    connect(ui_->boardView, SIGNAL(messageAccepted()), SLOT(slotStart()));

    // setup engine
    connect(engine_, SIGNAL(moveMade(TTT::Move)), SLOT(slotMoveMade(TTT::Move)));

    slotStart();
}


MainWindow::~MainWindow()
{
    delete ui_;
}



void MainWindow::slotStart()
{
    board_.init();
    if (board_.stm() != playerStm_)
        board_.flipStm();

    ui_->boardView->setBoard(board_);
}

void MainWindow::slotMoveMade(TTT::Move s)
{
    if (s == TTT::InvalidMove)
    {
        // means engine gives up (That's an error actually!)
        if (board_.stm() == engineStm_)
        {
            ui_->boardView->message("I'm lost!");
        }
        return;
    }

    // apply move
    board_.makeMove(s);
    board_.flipStm();

    ui_->boardView->setBoard(board_);

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
        ui_->boardView->message(m);
        //slotStart();
        return;
    }

    // engine's turn?
    if (board_.stm() == engineStm_)
    {
        engine_->setBoard(board_);
    }
}

