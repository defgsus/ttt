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

#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QThread>

#include "engine/board.h"

namespace TTT {
class Search;
}

static const int whatever = qRegisterMetaType<TTT::Move>("TTT::Move");

class Engine : public QThread
{
    Q_OBJECT

public:
    explicit Engine(QObject *parent = 0);
    ~Engine();

    bool thinking() const { return QThread::isRunning(); }

    /** Stop execution (async) */
    void stop();

    void setMaxDepth(int d) { maxDepth_ = d; }

    TTT::Search& ai() { return *ai_; }

signals:

    /** Engine has a move */
    void moveMade(TTT::Move);

public slots:

    /** Sets a position and starts search */
    void setBoard(const TTT::Board& b);

    virtual void run();

private:

    TTT::Search * ai_;
    TTT::Board board_;

    /** minimum move time for ai in ms */
    int minWaitTime_,
    /** max search depth in half-moves */
        maxDepth_;
};

#endif // ENGINE_H
