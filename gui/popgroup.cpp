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
    :   QWidget (parent),
        sel_    (0)
{
    layout_ = new QVBoxLayout(this);
    layout_->setMargin(0);

}

void PopGroup::addWidget(PopWidget * w)
{
    widgets_.push_back(w);

    const int i = widgets_.size() - 1;

    if (i>0)
    {
        QLayout * l = new QHBoxLayout();
        ((QVBoxLayout*)layout_)->addLayout(l);

        PopButton * b = new PopButton(PopButton::Up, this);
        l->addWidget(b);
        buttons_.push_back(b);

        connect(b, &PopButton::clicked, [i,this]()
        {
            if (sel_ == i && i>0)
                setActive(i-1);
            else
                setActive(i);
        });
    }

    layout_->addWidget(w);

    if (i != sel_)
        w->setVisible(false);
}


void PopGroup::setActive(int sel)
{
    if (sel_ == sel) return;

    widgets_[sel_]->setVisible(false);

    if (sel_ < (int)buttons_.size())
    {
        buttons_[sel_]->setDir(PopButton::Down);
    }

    sel_ = sel;

    widgets_[sel_]->setVisible(true);

    if (sel_ < (int)buttons_.size())
    {
        buttons_[sel_]->setDir(PopButton::Up);
    }

}
