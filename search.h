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

class Search
{
public:
    Search();

    /** Returns best move for current board and stm */
    Move bestMove(const Board& b, int * score = 0);

    void printTree(bool bestOnly = false, int maxlevel = -1, std::ostream& out = std::cout);

protected:

    static const int max_depth_ = 4;

    struct Node
    {
        int depth;
        bool ismax;
        // score/utility
        int x;
        // Node's board position
        Board board;
        // all possible moves
        Moves moves;
        // move that led to this node
        Move move;
        // child nodes
        std::vector<Node> childs;
        // best index into childs/moves
        unsigned int best;
        // terminal node?
        bool term;

        Node()
            : depth(0),ismax(false),move(InvalidMove),best(-1),term(false)
        { }
    };

    /** Evaluates a Node.
        Only needs depth and board to be set. */
    void minimax(Node& n);

    void printNode(const Node& n, bool bestOnly, int maxlevel, std::ostream& out = std::cout);

    Node root_;
};

#endif // SEARCH_H
