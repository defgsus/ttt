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
#ifdef TTT_THREADED
#   include <thread>
#   include <deque>
#endif

#include <QTime>


Search::Search()
{
}


Move Search::bestMove(Board& b, int maxd, int * score)
{
    // ---------- start search -----------

    if (score) *score = 0;

    // setup search stuff
    max_depth_ = maxd;
    Info info;
    alpha_ = -MaxScore*2;
    beta_ = MaxScore*2;

    // prepare root node
    root_.childs.clear();
    root_.depth = 0;
    root_.board = b;
    root_.ismax = true;
    root_.move = InvalidMove;
    root_.x = InvalidScore;
    root_.best = InvalidMove;
#ifdef TTT_ALPHA_BETA
    root_.alpha = alpha_;
    root_.beta = beta_;
#endif
    b.getMoves(root_.moves);
    if (root_.moves.empty()) return InvalidMove;

    // -- go --

    QTime time;
    time.start();

    std::cout << "!";
    std::cout.flush();

#ifndef TTT_THREADED
    // process single root
    minimax(&info, &root_);
#else
    // prepare child node for each thread
    std::vector<Info*> infos;
    for (uint i=0; i<root_.moves.size(); ++i)
    {
        root_.childs.push_back(Node());
        Node * c = &root_.childs[i];
        Info * info = new Info;
        infos.push_back(info);

        // setup node
        c->depth = root_.depth + 1;
        c->ismax = !root_.ismax;
        c->move = root_.moves[i];
#ifdef TTT_ALPHA_BETA
        c->alpha = root_.alpha;
        c->beta = root_.beta;
#endif
        ++info->num_nodes;

        // advance game
        c->board = root_.board;
        c->board.makeMove(c->move);
        c->board.flipStm();
    }
    // execute
    std::deque<std::thread> threads;
    for (uint i = 0; i<root_.childs.size(); ++i)
    {
        if (threads.size() >= 8)
        {
            threads.front().join();
            threads.pop_front();
            std::cout << "*";
            std::cout.flush();
        }
        threads.push_back( std::thread(std::bind(&Search::minimax, this,
                                infos[i], &root_.childs[i])));
    }
    // catch all threads
    for (size_t i=0; i<threads.size(); ++i)
    {
        threads[i].join();
        std::cout << "*";
        std::cout.flush();
    }

    // get best child
    for (size_t i=0; i<root_.childs.size(); ++i)
    {
        if (root_.x == InvalidScore ||
                root_.childs[i].x > root_.x)
        {
            root_.x = root_.childs[i].x;
            root_.best = i;
        }
    }

    for (auto i : infos)
    {
        info.combine(*i);
        delete i;
    }
#endif

    float took = (float)time.elapsed()/1000;

    std::cout << "\n"
              << " nodes: " << info.num_nodes
              << " prune: " << info.num_prune
#ifdef TTT_TRANSPOSITION_TABLE
              << " cache: " << info.num_cache_reuse
#endif
              << " level: " << info.num_level
           << "\n  took: " << took << "s"
              << " nps: " << (float)info.num_nodes/took
              << std::endl;

    // update board's eval map
    b.clearEvalMap();
    for (size_t i=0; i<root_.childs.size(); ++i)
        if (root_.childs[i].x != InvalidScore)
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




void Search::minimax(Info * info, Node * n)
{
    info->num_level = std::max(info->num_level, (uint)n->depth);

    if (n->ismax)
    {
        n->x = -MaxScore*2;
    }
    else
    {
        n->x = MaxScore*2;
    }

    n->board.getMoves(n->moves);
    n->best = InvalidMove;

    int eval = n->board.eval();
    if (!n->ismax) eval = -eval;

    // terminal node or max-depth?
    n->term = n->moves.empty() || abs(eval) >= WinScore;
    //if (n->term) std::cout << "term " << n->term << " depth " << n->depth << std::endl;
    if (n->term || n->depth >= max_depth_)//(std::max(2, (int)max_depth_ - (int)n->moves.size()/2)/2)*2)
    {
        n->x = eval;
        return;
    }

    for (size_t i=0; i<n->moves.size(); ++i)
    {
    #ifdef TTT_KEEP_TREE
        // create child node
        n->childs.push_back(Node());
        Node * c = &n->childs.back();
    #else
        Node * c;
        if (n->depth == 0)
        {
            n->childs.push_back(Node());
            c = &n->childs.back();
        }
            else c = new Node;
    #endif
        // setup node
        c->depth = n->depth + 1;
        c->ismax = !n->ismax;
        c->move = n->moves[i];
#ifdef TTT_ALPHA_BETA
        c->alpha = n->alpha;
        c->beta = n->beta;
#endif
        ++info->num_nodes;

        // advance game
        c->board = n->board;
        c->board.makeMove(n->moves[i]);
        c->board.flipStm();

        int score = 0;

#ifdef TTT_TRANSPOSITION_TABLE
        Hash hash;
        c->board.getHash(hash);
        auto ic = info->cache.find(hash);

        if (ic == info->cache.end())
        {
#endif
            // evaluate the child
            minimax(info, c);

            score = c->x;

#ifdef TTT_TRANSPOSITION_TABLE

            info->cache.insert( std::make_pair(hash, score) );
        }
        else
        {
            score = ic->second;
            info->num_cache_reuse++;
        }
#endif

#ifndef TTT_KEEP_TREE
        if (n->depth>0) delete c;
#endif

        // get score back
        if (n->ismax)
        {
            if (score > n->x) { n->x = score; n->best = i; }
#ifdef TTT_ALPHA_BETA
            if (n->x >= n->beta) { info->num_prune++; /*std::cout << "beta " << beta_ << std::endl;*/ break; }
            n->alpha = std::max(n->alpha, n->x);
#endif
        }
        else
        {
            if (score < n->x) { n->x = score; n->best = i; }
#ifdef TTT_ALPHA_BETA
            if (n->x <= n->alpha) { info->num_prune++; /*std::cout << "alpha " << alpha_ << std::endl;*/ break; }
            n->beta = std::min(n->beta, n->x);
#endif
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
