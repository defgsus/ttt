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

#include <QIcon>

#include "popbutton.h"


PopButton::PopButton(Dir dir, QWidget *parent)
    :   QPushButton(parent),
        w_    (32),
        h_    (32)

{
    setFlat(true);

    setFixedSize(w_, h_);    

    QString pic;
    switch (dir)
    {
        case Left: pic = "left"; break;
        case Right: pic = "right"; break;
        case Up: pic = "up"; break;
        case Down: pic = "down"; break;
    }

    setIcon(QIcon(":/img/b_" + pic + ".png"));
}


