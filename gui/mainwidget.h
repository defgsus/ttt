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

#include <vector>

#include <QWidget>

#include "engine/board.h"
#include "engine/boardhelper.h"

class Engine;
class BoardView;
class PopButton;

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

    /** Push Board on stack */
    void push();

    /** Pop Board from stack */
    void back();
    /** Pop Board from stack */
    void forward();

protected:

    void updateStackButtons_();


    BoardView * boardView_;
    PopButton * b_back_, * b_fwd_;

    TTT::Board board_;
    std::vector <TTT::Board> stack_;
    /** on insert pos, or pushed + 1 */
    size_t stack_pos_;
    TTT::BoardHelper helper_;
    Engine * engine_;

    TTT::Stm playerStm_, engineStm_;
};

#endif // MAINWIDGET_H
