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

#include "engine.h"
#include "engine/search.h"

#include <QTimer>

Engine::Engine(QObject *parent)
    :   QObject (parent),
        thread_ (this),
        ai_     (new TTT::Search)
{
    connect(&thread_, SIGNAL(started()), SLOT(slotStarted_()));
}

Engine::~Engine()
{
    thread_.quit();
    thread_.wait();

    delete ai_;
}

void Engine::slotStarted_()
{
    TTT::Move m = ai_->bestMove(board_, 4);
    //thread_.msleep(100);
    emit moveMade(m);
    thread_.quit();
    thread_.wait();

}

void Engine::setBoard(const TTT::Board &b)
{
    board_ = b;
    thread_.start();
}
