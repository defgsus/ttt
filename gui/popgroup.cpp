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
#include <QPalette>

#include "popgroup.h"
#include "popwidget.h"
#include "popbutton.h"


PopGroup::PopGroup(QWidget *parent)
    :   QWidget(parent)
{
    layout_ = new QVBoxLayout(this);
    layout_->setMargin(0);

    /*
        widget_->setParent(this);
        layout_->addWidget(widget_);

        QLayout * layout0_ = new QHBoxLayout;
        ((QVBoxLayout*)layout_)->addLayout(layout0_);

            b_ = new PopButton(this);
            layout0_->addWidget(b_);
    */
}

void PopGroup::addWidget(PopWidget * w)
{
    widgets_.push_back(w);

    QLayout * l = new QHBoxLayout(w);
    ((QVBoxLayout*)layout_)->addLayout(l);

        PopButton * b = new PopButton(PopButton::Left, this);
        l->addWidget(b);

        l->addWidget(w);
}
