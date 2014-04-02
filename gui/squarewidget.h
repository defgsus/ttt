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

#ifndef GUI_SQUAREWIDGET_H
#define GUI_SQUAREWIDGET_H

#include <QWidget>
#include <QPen>
#include <QBrush>

class SquareWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SquareWidget(QWidget *parent = 0);

    void setSelected(bool sel) { bool u = sel != selected_; selected_ = sel; if (u) update(); }
    bool selected() const { return selected_; }

signals:

    void clicked();

public slots:

protected:

    virtual void resizeEvent(QResizeEvent *);

    virtual void paintEvent(QPaintEvent *);

    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void mousePressEvent(QMouseEvent *);

    bool hover_;
    bool selected_;

    QBrush br_back_, br_hover_;
    QPen pen_select_;
};

#endif // GUI_SQUAREWIDGET_H
