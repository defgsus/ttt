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
    The NegaMax template class represents a negamax (alpha/beta) search algorithm
    for any kind of nodes.

    <p>The Node template parameter should represent a struct/class with
    about the following interface:</p>

    @code
    struct Node
    {
        typedef <x> Score;           // integer type for scores
        typedef <x> Index;           // integral type for (child) indices

        static Score maxScore();     // should return the maximum possible score

        Score evaluate() const;      // should return an evaluated score
                                     // the score should be relative to the side-to-move
                                     // (e.g. positive for Max-Nodes, negative for Min-Nodes)

        //bool isTerminal() const;   // should return if there can be successors.
                                     // (NOT USED CURRENTLY! Would be inefficient.)

        void createChilds();         // should create successor nodes (always called before numChilds)

        Index numChilds() const;     // should return number of successors

        Node child(Index i) const;   // should return a child node for each index

        void setBestChild(Node * c); // will be called frequently, when a child node is found current best
    };
    @endcode

 **/
template <class Node>
class NegaMax
{
public:
    /** Type of score values */
    typedef typename Node::Score Score;
    /** Type of index values */
    typedef typename Node::Index Index;

    /** Searches the tree */
    void search(int maxDepth, Node * n);
    /** Searches the tree with alpha/beta pruning */
    void search_ab(int maxDepth, Node * n);

    /** Returns number of evaluated nodes */
    int numNodes() const { return nodes_; }
    /** Returns number of pruned nodes. */
    int numPrunes() const { return prunes_; }

private:

    /** Initializes counters and stuff */
    void init_();

    /** Negamax recusive function */
    Score negamax_(int depth, Node * n);
    /** Negamax recusive function with a/b pruning. */
    Score negamax_(int depth, Score alpha, Score beta, Node * n);

    /** evaluated nodes */
    int nodes_,
    /** pruned nodes */
        prunes_;
};



// ------------------- implementation ---------------------

template <class Node>
void NegaMax<Node>::init_()
{
    nodes_ = 0;
    prunes_ = 0;
}

template <class Node>
void NegaMax<Node>::search(int maxDepth, Node * n)
{
    init_();
    negamax_(maxDepth, n);
}

template <class Node>
void NegaMax<Node>::search_ab(int maxDepth, Node * n)
{
    init_();
    negamax_(maxDepth, -Node::maxScore(), Node::maxScore(), n);
}


template <class Node>
typename NegaMax<Node>::Score NegaMax<Node>::negamax_(int depth, Node * n)
{
    nodes_++;

    if (depth <= 0)
    {
        n->score = n->evaluate();
        return n->score;
    }

    Score maxv = -Node::maxScore();

    n->createChilds();

    // terminal?
    if (!n->numChilds())
    {
        return n->score = n->evaluate();
    }

    for (Index i = 0; i < n->numChilds(); ++i)
    {
        Node c = n->child(i);
        const Score score = -negamax_(depth - 1, &c);

        if (score > maxv)
        {
            n->score = maxv = score;
            n->setBestChild(&c);
        }
    }

    return n->score = maxv;
}


template <class Node>
typename NegaMax<Node>::Score NegaMax<Node>::negamax_(int depth, Score alpha, Score beta, Node * n)
{
    nodes_ ++;

    if (depth <= 0)
    {
        n->score = n->evaluate();
        return n->score;
    }

//    if (n->isTerminal())
//        return n->evaluate();

    n->createChilds();

    // terminal?
    if (!n->numChilds())
    {
        return n->score = n->evaluate();
    }

    for (Index i = 0; i < n->numChilds(); ++i)
    {
        Node c = n->child(i);

        const Score score = -negamax_(depth - 1, -beta, -alpha, &c);

        if (score >= beta)
        {
            prunes_ ++;
            n->score = score;
            n->setBestChild(&c);
            return beta;
        }

        if (score > alpha)
        {
            n->score = alpha = score;
            n->setBestChild(&c);
        }
    }

    n->score = alpha;
    return alpha;
}



} // namespace TTT

#endif // TTT_ENGINE_NEGAMAX_H
