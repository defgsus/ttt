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

#include "particles.h"



Particles::Particles(QObject *parent) :
    QObject(parent)
{
    timer_.setInterval(1000 / 60);
    timer_.setSingleShot(false);

    connect(&timer_, SIGNAL(timeout()), SLOT(forward_(qreal)));
}


void Particles::addParticles(const QPointF& where, qreal size, int num)
{
    for (int i=0; i<num; ++i)
    {
        QPointF dir((qreal)rand()/RAND_MAX-0.5, (qreal)rand()/RAND_MAX-0.5);
        addParticle_(where, dir, size);
    }
}


void Particles::addParticle_(const QPointF &where, const QPointF &dir, qreal size)
{
    Particle * p = 0;
    for (int i=0; i<p_.size(); ++i)
    {
        if (!p_[i].active)
        {
            p = &p_[i];
            break;
        }
    }

    if (!p)
    {
        p_.push_back(Particle());
        p = &p_.back();
    }

    p->active = true;
    p->pos = where;
    p->dir = dir;
    p->lifetime = 3;
    p->size = size;
}

void Particles::forward_(qreal delta)
{
    bool did_something = false;

    for (int i=0; i<p_.size(); ++i)
    if (p_[i].active)
    {
        // lifetime
        p_[i].lifetime -= delta;
        if (p_[i].lifetime <= 0)
            p_[i].active = false;

        did_something = true;

        // impulse
        p_[i].pos += delta * p_[i].dir;

        // damping
        p_[i].dir *= (1.0 - delta * 0.05);

        // gravity
        p_[i].dir.setY(p_[i].dir.ry() + 0.2 * delta);
    }

    if (did_something)
        emit needDraw();
}


void Particles::draw(QPainter& p)
{
    for (int i=0; i<p_.size(); ++i)
    if (p_[i].active)
    {
        qreal s = p_[i].size;
        p.drawEllipse(p_[i].pos, s, s);
    }
}
