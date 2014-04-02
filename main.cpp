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

#include <QApplication>

#include <QDesktopWidget>
#include <QWidget>

#include "gui/settings.h"
#include "gui/mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AppSettings = new Settings;

    MainWidget w;

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN) || defined(Q_OS_MAC)
    // set some size
    w.setGeometry(0,0, 500, 500);

    // center
    QRect r = QApplication::desktop()->rect();
    w.setGeometry((r.width() - w.width())/2, (r.height() - w.height()) / 2,
                  w.width(), w.height());
#endif

    w.show();

    return a.exec();

    delete AppSettings;
}
