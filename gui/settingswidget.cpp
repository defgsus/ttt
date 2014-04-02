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
#include "settings.h"

SettingsWidget::SettingsWidget(QWidget *parent)
    :   PopWidget(parent)
{
    QVBoxLayout * l = new QVBoxLayout(this);

#define TTT_GUI_ADD_NUM(widget__, key__, label__, min__, max__)      \
    widget__ = new NumberWidget(label__, min__, min__, max__, this); \
    l->addWidget(widget__);                                          \
    connect(widget__, &NumberWidget::numberChanged, [this]()         \
    {                                                                \
        AppSettings->setValue(key__, widget__->getNumber());         \
        emit changed();                                              \
    });

    TTT_GUI_ADD_NUM(n_size_,    "size",     "board size (N)", 3, 10);
    TTT_GUI_ADD_NUM(n_rows_,    "cons",     "row length (X)", 3, 10);
    TTT_GUI_ADD_NUM(n_depth_,   "depth",    "search depth",   1, 8);

#undef TTT_GUI_ADD_NUM

    slotReconfigure();
}


void SettingsWidget::slotReconfigure()
{
    n_size_->setNumber(AppSettings->getValue("size").toInt());
    n_rows_->setNumber(AppSettings->getValue("cons").toInt());
    n_depth_->setNumber(AppSettings->getValue("depth").toInt());
}
