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

    Node(const Board& b, BoardHelper * helper, Node * parent = 0)
        :   greed(-TTT::MaxScore),
            parent(parent), score(0), depth(0), board(b), helper(helper),
            bestChildMove(InvalidMove), isEvaluated(false)
    { }

    Node()
        :   score(0), depth(0), bestChildMove(InvalidMove), isEvaluated(false)
    { }

    //Node(const Node&);

    // ---- interface ----

    Score evaluate();

    bool isTerminal();

    bool createChilds();
    Index numChilds() const;
    Node getChild(Index i);
    void childEvaluated(const Node * c);
    void setBestChild(const Node * c);

    // ---- helper ----

    Score forceEval();

    // ----- member -----

    // -- config --

    int greed;

    // -- during search --

    Node * parent;

    Score score;
    Moves moves;
    int depth;

    Board board;
    BoardHelper * helper;

    Move move,
        bestChildMove;

    bool isEvaluated;
    Score evaluation;

};

/*
Node::Node(const Node &n)
    :   score   (0),
        depth   (n.depth),
        board   (n.board),
        helper  (n.helper),
        bestChildMove(InvalidMove),
        isEvaluated(false)
{

}
*/

inline Node::Score Node::evaluate()
{
    const Score s = isEvaluated? evaluation : forceEval();
    //return std::max(-WinScore,std::min(WinScore, s ));
    return s;
}

inline bool Node::isTerminal()
{
    return (abs(evaluate()) >= WinScore);
}

inline Node::Score Node::forceEval()
{
    evaluation = helper->eval(board);

    // rate close wins higher
    //evaluation /= (depth + 1);

    isEvaluated = true;
    return evaluation;
}

inline bool Node::createChilds()
{
    helper->getMoves(board, moves);
    return !moves.empty();
}

inline Node::Index Node::numChilds() const { return moves.size(); }

inline Node Node::getChild(Index i)
{
    Node n(*this);
    n.isEvaluated = false;
    n.move = moves[i];
    n.board.makeMove(n.move);
    //TTT_DEBUG(n.board.numAllCaptured());
    n.board.flipStm();
    n.depth++;
//        TTT_DEBUG(std::setw(depth) << "" << (depth&1? "min " : "max ") << board.toString(n.move));
    return n;
}

inline void Node::childEvaluated(const Node * c)
{
    if (depth == 0)
        board.setEvalMap(c->move, -c->score);
}

inline void Node::setBestChild(const Node * c)
{
    //TTT_DEBUG(std::setw(depth) << "" << (depth&1? "min " : "max ") << board.toString(c->move) << " " << c->score);
    bestChildMove = c->move;
}



class Search
{
    public:

    Search()
        :   captureWeight(10),
#ifdef TTT_GREEDY
            greed(-MaxScore),
#endif
            helper_(3,3)
    { }

    /** Return best move for current side-to-move */
    Move bestMove(Board& b, int maxdepth);

    // ---------- public member ------------

    // ----- settings --

    int captureWeight;
#ifdef TTT_GREEDY
    int greed;
#endif

    // ----- stats -----

    /** last best score */
    int score;
    /** reached depth */
    int depth;
    /** evaluated nodes */
    int nodes;
    /** number of called evaluations */
    int evals;
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


inline Move Search::bestMove(Board &b, int maxdepth)
{
    // update helper size
    helper_.setSize(b);
    helper_.setCaptureWeight(captureWeight);

#ifdef TTT_LIMIT_DEPTH

    // adjust level to number of root nodes
    Moves moves;
    helper_.getMoves(b, moves);
    int numMoves = moves.size();

    if (numMoves > 35)
        maxdepth = std::min(maxdepth, 4);
    else
    if (numMoves > 25)
        maxdepth = std::min(maxdepth, 6);
    else
    if (numMoves > 15)
        maxdepth = std::min(maxdepth, 8);
    else
    if (numMoves > 10)
        maxdepth = std::min(maxdepth, 10);
#endif

    // setup root node
    Node n(b, &helper_);

    n.board.resetNumAllCaptured();
    n.board.clearEvalMap();
#ifdef TTT_GREEDY
    n.greed = greed;
#endif

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
    depth = search_.maxDepth();
    nodes = search_.numNodes();
    prunes = search_.numPrunes();
    evals = search_.numEvals();
    nps = (int)( (double)nodes / std::max(1, time) * 1000 );

    b.copyEvalFrom(n.board);
#ifndef TTT_NO_PRINT
    std::cout << std::endl;

    std::cout << " depth " << depth
              << " nodes " << nodes
              << " evals " << evals
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
