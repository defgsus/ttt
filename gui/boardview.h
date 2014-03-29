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

#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QWidget>
#include <QRect>
#include <QBrush>
#include <QPen>

#include "engine/board.h"

class BoardView : public QWidget
{
    Q_OBJECT
public:
    explicit BoardView(QWidget *parent = 0);
    ~BoardView();

    /** Sets a new position */
    void setBoard(const TTT::Board& b);

    // ------- event ------------
    
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    
signals:

public slots:


private:

    QRect squareRect(TTT::Square s) const;

    //TTT::Board board_;

    int margin_,
        sqmargin_,
        size_,
        sqs_, xo_, yo_;

    // --- pens and brushes ---

    QBrush background_;
    QPen xPen_, oPen_, cPen_;
};

#endif // BOARDVIEW_H
