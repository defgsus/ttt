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

#ifndef GUI_NUMBERWIDGET_H
#define GUI_NUMBERWIDGET_H

#include <vector>

#include <QWidget>


class SquareWidget;

/** small number selector */
class NumberWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NumberWidget(const QString& label, int num, int num_min, int num_max,
                          QWidget *parent = 0);

    int  getNumber() const { return num_; }

signals:

    void numberChanged(int num);

public slots:

    void setNumber(int num);

protected:

    int num_, min_, max_, len_;

    std::vector<SquareWidget*> squares_;
};

#endif // GUI_NUMBERWIDGET_H
