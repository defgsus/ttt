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
#include <QDebug>

/*
class EngineThread : public QThread
{
    Q_OBJECT
public:
    EngineThread(Engine * parent)
        :   QThread(parent),
          ai  (parent)
    {

    }

    virtual void run()
    {
        QTime t;
        t.start();

        TTT::Move m = ai->ai_->bestMove(ai->board_, 6);

        while (t.elapsed() < ai->minWaitTime_);

        emit moveMade(m);
    }

signals:

    void moveMade(TTT::Move);

    Engine * ai;
};
*/


Engine::Engine(QObject *parent)
    :   QThread (parent),
        ai_     (new TTT::Search),
        minWaitTime_    (350),
        maxDepth_   (4)
{

}

Engine::~Engine()
{
    delete ai_;
}


void Engine::run()
{
    qDebug() << "Engine::run()";

    QTime t;
    t.start();

    TTT::Move m = ai_->bestMove(board_, maxDepth_);

//#ifndef NDEBUG
    board_.printBoard(true);
//#endif

    int e = t.elapsed();

    // wait minimum time
    if (e<minWaitTime_)
        msleep(minWaitTime_ - e);

    emit moveMade(m);
}

void Engine::stop()
{
    // XXX TODO
}



void Engine::setBoard(const TTT::Board &b)
{
    qDebug() << "Engine::setBoard()";
    board_ = b;

    start();//thread_->start();
}
