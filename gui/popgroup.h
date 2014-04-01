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

#ifndef GUI_POPGROUP_H
#define GUI_POPGROUP_H

#include <vector>
#include <QWidget>

class QLayout;
class PopWidget;

/**  _____  widget
    |     | layout
    |__-__| layout


 */
class PopGroup : public QWidget
{
    Q_OBJECT
public:
    explicit PopGroup(QWidget *parent = 0);

    QLayout * layout() { return layout_; }

    void addWidget(PopWidget * w);

    PopWidget * getWidget(uint i) { return widgets_[i]; }
    const PopWidget * getWidget(uint i) const { return widgets_[i]; }

signals:

public slots:

protected:

    QLayout * layout_;
    std::vector<PopWidget *> widgets_;
};

#endif // GUI_POPGROUP_H
