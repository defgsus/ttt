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

#include "search.h"

#include <iomanip>

#include <QTime>

Search::Search()
{
}


Move Search::bestMove(Board& b, int maxd, int * score)
{
    // ---------- start search -----------

    if (score) *score = 0;

    max_depth_ = maxd;

    Moves m;
    b.getMoves(m);
    if (m.empty()) return InvalidMove;

    // prepare root node
    root_.childs.clear();
    root_.depth = 0;
    root_.board = b;
    root_.ismax = true;
    root_.move = InvalidMove;

    // setup other stuff
    num_nodes_ = 0;
    num_cache_reuse_ = 0;
    num_prune_ = 0;
    alpha_ = -MaxScore*2;
    beta_ = MaxScore*2;

    root_.alpha = alpha_;
    root_.beta = beta_;

#ifdef TTT_TRANSPOSITION_TABLE
    cache_.clear();
#endif
    QTime time;

    std::cout << "!";
    std::cout.flush();

    // go
    time.start();
    minimax(root_);

    float took = (float)time.elapsed()/1000;

    std::cout << " nodes: " << num_nodes_
              << " prune: " << num_prune_
#ifdef TTT_TRANSPOSITION_TABLE
              << " cache: " << num_cache_reuse_
#endif
              << " took: " << took << "s "
              << " nps: " << (float)num_nodes_/took
              << std::endl;

    // update board's eval map
    b.clearEvalMap();
    for (size_t i=0; i<root_.childs.size(); ++i)
        b.setEvalMap(root_.moves[i], root_.childs[i].x);

    if (root_.best == InvalidMove)
    {
        //std::cout << "no best move " << root_.childs.size() << std::endl;
        return InvalidMove;
    }

    if (score) *score = root_.x;

    return root_.moves[root_.best];
    //return m[rand() % m.size()]; // the other ai ;)
}


void Search::minimax(Node& n)
{
    if (n.ismax)
    {
        n.x = -MaxScore*2;
    }
    else
    {
        n.x = MaxScore*2;
    }

    n.board.getMoves(n.moves);
    n.best = InvalidMove;

    int eval = n.board.eval();
    if (!n.ismax) eval = -eval;

    // terminal node or max-depth?
    n.term = n.moves.empty() || abs(eval) >= WinScore;
    //if (n.term) std::cout << "term " << n.term << " depth " << n.depth << std::endl;
    if (n.term || n.depth >= max_depth_)
    {
        n.x = eval;
        if (n.depth&1 && n.board.stm() == O) n.x = -n.x;
        return;
    }

    for (size_t i=0; i<n.moves.size(); ++i)
    {
    #ifdef TTT_KEEP_TREE
        // create child node
        n.childs.push_back(Node());
        Node * c = &n.childs.back();
    #else
        Node * c;
        if (n.depth == 0)
        {
            n.childs.push_back(Node());
            c = &n.childs.back();
        }
            else c = new Node;
    #endif
        // setup node
        c->depth = n.depth + 1;
        c->ismax = !n.ismax;
        c->move = n.moves[i];
        c->alpha = n.alpha;
        c->beta = n.beta;
        ++num_nodes_;

        // advance game
        c->board = n.board;
        c->board.makeMove(n.moves[i]);
        c->board.flipStm();

        int score = 0;

#ifdef TTT_TRANSPOSITION_TABLE
        Hash hash;
        c->board.getHash(hash);
        auto ic = cache_.find(hash);

        if (ic == cache_.end())
        {
#endif
            // evaluate the child
            minimax(*c);

            score = c->x;

#ifdef TTT_TRANSPOSITION_TABLE

            cache_.insert( std::make_pair(hash, score) );
        }
        else
        {
            score = ic->second;
            num_cache_reuse_++;
        }
#endif

#ifndef TTT_KEEP_TREE
        // XXX quick hack for keeping the root, this leaks memory
        if (n.depth>0) delete c;
#endif

        // get score back
        if (n.ismax)
        {
            if (score > n.x) { n.x = score*0.7; n.best = i; }
            if (n.x >= n.beta) { num_prune_++; /*std::cout << "beta " << beta_ << std::endl;*/ break; }
            n.alpha = std::max(n.alpha, n.x);
        }
        else
        {
            if (score < n.x) { n.x = score*0.7; n.best = i; }
            if (n.x <= n.alpha) { num_prune_++; /*std::cout << "alpha " << alpha_ << std::endl;*/ break; }
            n.beta = std::min(n.beta, n.x);
        }
    }
}



void Search::printTree(bool bestOnly, int maxlevel, std::ostream& out)
{
    printNode(root_, bestOnly, maxlevel, out);
}

void Search::printNode(const Node &n, bool bestOnly, int maxlevel, std::ostream &out)
{
    out << std::setw(n.depth) << ""
        << (n.ismax? "MAX " : "MIN ")
        << "d=" << n.depth << " c=" << n.childs.size() << " x=" << n.x
        << " " << (n.move != InvalidMove? n.board.toString(n.move) : "-")
        //<< (n.invalid? " invalid" : "")
        //<< " " << (n.best<n.moves.size()? n.board.toString(n.moves[n.best]) : "-")
        ;
    if (n.term) out << " T";

    out << std::endl;

    if (maxlevel >= 0 && n.depth >= maxlevel) return;

    for (size_t i=0; i<n.childs.size(); ++i)
    if (!bestOnly || i == n.best)
    {
        //out << std::setw(n.depth) << " " << "\\";
        printNode(n.childs[i], bestOnly, maxlevel, out);
    }
    //out << std::endl;
}
