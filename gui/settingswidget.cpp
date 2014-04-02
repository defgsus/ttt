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

#include "settingswidget.h"
#include "numberwidget.h"

SettingsWidget::SettingsWidget(QWidget *parent)
    :   QWidget(parent)
{
    QVBoxLayout * l = new QVBoxLayout(this);

    auto n = new NumberWidget("search depth", 4, 1, 8, this);
    l->addWidget(n);

    n = new NumberWidget("board size (N)", 5, 3, 10, this);
    l->addWidget(n);

    n = new NumberWidget("row lenth (X)", 4, 3, 10, this);
    l->addWidget(n);

}
