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

#include "settings.h"

Settings * AppSettings;

Settings::Settings(QObject *parent) :
    QSettings("com.modular-audio-graphics", "XInARowInNSquare", parent)
{

}

const QMap<QString, QVariant>& Settings::defaultValues() const
{
    if (map_.empty())
    {
        map_.insert("size",     QVariant(5));
        map_.insert("cons",     QVariant(4));
        map_.insert("depth",    QVariant(4));
        map_.insert("capweight",QVariant(70));
    }
    return map_;
}

QVariant Settings::getValue(const QString& key) const
{
    const auto i = defaultValues().find(key);
    Q_ASSERT_X(i != map_.end(), "Settings", "key unkown");

    return value(key, i.value());
}
