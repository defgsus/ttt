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

#ifndef PARTICLES_H
#define PARTICLES_H

#include <QObject>
#include <QPointF>
#include <QVector>
#include <QPainter>
#include <QTimer>

class Particles : public QObject
{
    Q_OBJECT
public:
    explicit Particles(QObject *parent = 0);

signals:

    void needDraw();

public slots:

    void addParticles(const QPointF& where, qreal size_in_pixels, int num);

    void draw(QPainter& painter);

protected slots:

    void forward_(qreal delta);
    void onTimer_();

protected:

    void addParticle_(const QPointF& where, const QPointF& dir, qreal size);

    struct Particle
    {
        bool active;
        QPointF
            pos, dir;
        qreal size, lifetime;
    };

    QVector<Particle> p_;

    QTimer timer_;
};

#endif // PARTICLES_H
