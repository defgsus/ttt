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
#include <QFont>
#include <QTimer>

#include "engine/board.h"

class Particles;

class BoardView : public QWidget
{
    Q_OBJECT
public:
    explicit BoardView(QWidget *parent = 0);
    ~BoardView();

    /** Sets a new position */
    void setBoard(const TTT::Board& b);

    // ------- event ------------
    
    virtual void resizeEvent(QResizeEvent *);
    virtual void paintEvent(QPaintEvent *);
    
    virtual void leaveEvent(QEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *);

signals:

    /** Signals that user has made a move */
    void moveMade(TTT::Move s);

public slots:

    /** display a message */
    void message(const QString&);

    /** update after @p ms millisecs */
    void updateIn(int ms);

    void addParticles(TTT::Square s);

protected:
    // ______ PROTECTED FUNCS _______

    QRect squareRect(TTT::Square s) const;

    /** Returns the square for mouse coords, or TTT::InvalidMove */
    TTT::Square squareAt(int x, int y) const;

    /** Returns wheter the square can be set. */
    bool canMoveTo(TTT::Square s) const;

    // ____________ MEMBER __________

    enum PaintWhat
    {
        Nothing,
        PlayerX,
        PlayerO,
        Locked1,
        Locked2
    };

    struct PaintSquare
    {
        PaintWhat what;
    };

    TTT::Board board_;

    /** What to paint */
    std::vector<PaintSquare> boardp_;

    int margin_,
        sqmargin_;
    size_t size_;
    int sqs_, xo_, yo_;

    // ---- gui interaction

    int hoverSquare_;

    QString message_;
    int showMessage_;

    QTimer updateTimer_;

    Particles * particles_;

    // --- pens and brushes ---

    QBrush background_, bBrush_, bBrushH_;
    QPen xPen_, oPen_, cPen_, mPen_;
    QFont mFont_;
};

#endif // BOARDVIEW_H
