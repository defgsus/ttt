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

#ifndef TTT_ENGINE_SEARCH_H
#define TTT_ENGINE_SEARCH_H

#include <QTime>

#include "board.h"
#include "boardhelper.h"
#include "negamax.h"

namespace TTT {


/** Implementation of a NegaMax Node for playing <X in a row> */
struct Node
{
    typedef int Score;
    typedef int Index;

    static Score maxScore() { return MaxScore; }

    Node(const Board& b, BoardHelper * helper)
        :   score(0), depth(0), board(b), helper(helper), bestChildMove(InvalidMove)
    { }

    Node()
        :   score(0), depth(0), bestChildMove(InvalidMove)
    { }

    Score evaluate() const;

    bool isTerminal() const;

    void createChilds();
    Index numChilds() const;
    Node child(Index i) const;
    void setBestChild(Node * c);

    // ----- member -----

    Score score;
    Moves moves;
    int depth;

    Board board;
    BoardHelper * helper;

    Move move,
        bestChildMove;

};

Node::Score Node::evaluate() const
{
    const Score s = helper->eval(board);
    return depth & 1 ? -s : s;
}

bool Node::isTerminal() const
{
    return helper->isOver(board);
}

void Node::createChilds()
{
    helper->getMoves(board, moves);
    TTT_DEBUG("moves " << moves.size());
}

Node::Index Node::numChilds() const { return moves.size(); }

Node Node::child(Index i) const
{
    Node n(*this);
    n.move = moves[i];
    n.board.makeMove(n.move);
    n.board.flipStm();
    n.depth++;
//        TTT_DEBUG(std::setw(depth) << "" << (depth&1? "min " : "max ") << board.toString(n.move));
    return n;
}

void Node::setBestChild(Node * c)
{
    //TTT_DEBUG(std::setw(depth) << "" << (depth&1? "min " : "max ") << board.toString(c->move) << " " << c->score);
    //if (depth == 1)
        //board.setEvalMap(c->move, c->score);
    bestChildMove = c->move;
}



class Search
{
    public:

    Search() : helper_(3,3) { }

    /** Return best move for current side-to-move */
    Move bestMove(const Board& b, int maxdepth);

    // ---------- public member ------------

    /** last best score */
    int score;
    /** evaluated nodes */
    int nodes;
    /** number of pruned nodes */
    int prunes;
    /** time of last search in milliseconds */
    int time;
    /** archived nodes per seconds */
    int nps;


private:

    NegaMax<Node> search_;

    BoardHelper helper_;
};


Move Search::bestMove(const Board &b, int maxdepth)
{
    // update helper size
    helper_.setSize(b);

    // setup root node
    Node n(b, &helper_);

#ifndef TTT_NO_PRINT
    std::cout << "!";
#endif

    // search (and messure time)

    QTime ti;
    ti.start();

#ifdef TTT_ALPHA_BETA
    search_.search_ab(maxdepth, &n);
#else
    search_.search(maxdepth, &n);
#endif

    // stats
    time = ti.elapsed();
    score = n.score;
    nodes = search_.numNodes();
    prunes = search_.numPrunes();
    nps = (int)( (double)nodes / std::max(1, time) * 1000 );

#ifndef TTT_NO_PRINT
    std::cout << std::endl;

    std::cout << " depth " << maxdepth
              << " nodes " << nodes
#ifdef TTT_ALPHA_BETA
              << " prunes " << prunes
#endif
              << " nps " << nps
              << " took " << ((double)time/1000) << "s"
              << std::endl;
#endif

    return n.bestChildMove;
}



} // namespace TTT

#endif // TTT_ENGINE_SEARCH_H
