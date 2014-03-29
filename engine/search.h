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

#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "boardhelper.h"

#ifdef TTT_TRANSPOSITION_TABLE
#include <map>
#endif

namespace TTT {

class Search
{
public:
    Search();

    /** Returns best move for current board and stm.
        Also sets Board's evalMap */
    Move bestMove(Board& b, int max_depth, int * score = 0);

#ifdef TTT_GREEDY
    /** Sets the greedyness.
        A positive value means, the engine discards nodes that are
        not at least @p greed points better than parent branch.
        A negative value means, the engine discards nodes that are
        worse then abs(@p greed) points than the parent node. */
    void greed(int greed) { greed_ = greed; }
    int  greed() const { return greed_; }
#endif

    void printTree(bool bestOnly = false, int maxlevel = -1, std::ostream& out = std::cout);

protected:

    int max_depth_;

    struct Node
    {
        Node * parent;
        int depth;
        bool ismax;
        // score/utility
        int x;
        // Node's board position
        Board board;
        // all possible moves
        Moves moves;
        // move which led to this node
        Move move;
        // child nodes
        Node * childs;
        size_t numChilds;
        // best index into childs/moves
        uint best;
        // terminal node?
        bool term;

        Node()
            : depth(0),ismax(false),move(InvalidMove),childs(0),numChilds(0),best(-1),term(false)
        { }

        ~Node()
        {
            if (childs) free(childs);
        }

        void init()
        {
            depth = 0; ismax=false; move=InvalidMove; childs=0; numChilds=0; best=-1; term=false;
        }

        void allocChilds(size_t num)
        {
            childs = (Node*)calloc(num, sizeof(Node));
            numChilds = num;
        }

        void freeChilds()
        {
            if (childs) delete childs;
            childs = 0; numChilds = 0;
        }
    };

    /** info per node-thread */
    struct Info
    {
        Info() :
#ifdef TTT_ALPHA_BETA
                alpha(-MaxScore), beta(MaxScore),
#endif
                num_nodes(0), num_cache_reuse(0), num_prune(0),
                num_cuts(0), num_level(0)
            { }

        void combine(const Info& r)
        {
            num_nodes += r.num_nodes;
            num_cache_reuse += r.num_cache_reuse;
            num_prune += r.num_prune;
            num_cuts += r.num_cuts;
            num_level = std::max(num_level, r.num_level);
        }

#ifdef TTT_ALPHA_BETA
        int alpha, beta;
#endif

        uint num_nodes,
             num_cache_reuse,
             num_prune,
             num_cuts,
             num_level;
#ifdef TTT_TRANSPOSITION_TABLE
            std::map<Hash,int> cache;
#endif
    };

    /** Evaluates a Node.
        Only needs depth and board to be set. */
    void minimax(Info * info, Node * n);

    void printNode(const Node& n, bool bestOnly, int maxlevel, std::ostream& out = std::cout);

    // ____________ MEMBER _____________

    BoardHelper helper_;

    Node root_;

#ifdef TTT_GREEDY
    int greed_;
#endif

};


} // namespace TTT


#endif // SEARCH_H
