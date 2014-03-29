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

#ifndef TTT_ENGINE_NEGAMAX_H
#define TTT_ENGINE_NEGAMAX_H

#include "common.h"

namespace TTT {


/**
 *
 *
 *  Node::evaluate() must return score relative to side to move (e.g. + for White, - for Black)
 *
 **/
template <class Node>
class NegaMax
{
public:
    /** Type of score values */
    typedef typename Node::Score Score;
    /** Type of index values */
    typedef typename Node::Index Index;

    void search(int maxDepth, Node * n);

    Score negamax(int depth, Node * n);
    Score negamax(int depth, Score alpha, Score beta, Node * n);

    Score alpha, beta;
};



// ------------------- implementation ---------------------


template <class Node>
void NegaMax<Node>::search(int maxDepth, Node * n)
{
    alpha = -Node::maxScore();
    beta = Node::maxScore();

    negamax(maxDepth, alpha, beta, n);
}


template <class Node>
typename NegaMax<Node>::Score NegaMax<Node>::negamax(int depth, Node * n)
{
    if (depth <= 0)
    {
        n->score = n->evaluate();
        return n->score;
    }

    Score maxv = -Node::maxScore();

    n->createChilds();
    for (Index i = 0; i < n->numChilds(); ++i)
    {
        Node c = n->child(i);
        const Score score = -negamax(depth - 1, &c);

        if (score > maxv)
        {
            maxv = score;
            n->setBestChild(&c);
        }
    }

    return maxv;
}


template <class Node>
typename NegaMax<Node>::Score NegaMax<Node>::negamax(int depth, Score alpha, Score beta, Node * n)
{
    if (depth <= 0)
    {
        n->score = n->evaluate();
        return n->score;
    }

    n->createChilds();
    for (Index i = 0; i < n->numChilds(); ++i)
    {
        Node c = n->child(i);

        const Score score = -negamax(depth - 1, -beta, -alpha, &c);

        if (score >= beta)
        {
            n->setBestChild(&c);
            return beta;
        }

        if (score > alpha)
        {
            alpha = score;
            n->setBestChild(&c);
        }
    }

    return alpha;
}



} // namespace TTT

#endif // TTT_ENGINE_NEGAMAX_H
