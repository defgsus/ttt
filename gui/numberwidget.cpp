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

#include <QLayout>
#include <QLabel>

#include "numberwidget.h"
#include "squarewidget.h"

NumberWidget::NumberWidget(const QString& label, int num, int num_min, int num_max,
                           QWidget *parent)
    :   QWidget(parent),
      num_    (num),
      min_    (num_min),
      max_    (num_max),
      len_    (num_max - num_min)
{
    QHBoxLayout * l = new QHBoxLayout(this);

    for (int i = 0; i < len_; ++i)
    {
        SquareWidget * s = new SquareWidget(this);

        l->addWidget(s);
        squares_.push_back(s);
    }
}
