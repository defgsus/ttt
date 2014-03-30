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

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QWidget>
#include <QString>
#include <QBrush>

class MessageBox : public QWidget
{
    Q_OBJECT
public:
    explicit MessageBox(const QString& message, QWidget *parent = 0);

signals:

public slots:

protected:
    virtual void paintEvent(QPaintEvent*);
    virtual void mousePressedEvent(QMouseEvent *);

    QString message_;

    QBrush bBrush_;
};

#endif // MESSAGEBOX_H
