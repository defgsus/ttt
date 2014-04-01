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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QThread>

#include "engine/board.h"
#include "engine/boardhelper.h"

class Engine;
class BoardView;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);

signals:

public slots:

    /** Move from user */
    void slotMoveMade(TTT::Move s);

    /** Start new game */
    void slotStart();

protected:

    BoardView * boardView_;

    TTT::Board board_;
    TTT::BoardHelper helper_;
    Engine * engine_;

    QThread * engineThread_;

    TTT::Stm playerStm_, engineStm_;
};

#endif // MAINWIDGET_H
