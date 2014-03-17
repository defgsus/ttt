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

#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <iostream>

typedef unsigned char Piece;
typedef unsigned int Move;
typedef std::vector<Move> Moves;

const Move InvalidMove = -1;

enum PieceType
{
    Empty, X, O
};

class Board
{
public:
    Board(unsigned int size = 3);

    /** Clears board and sets stm to X */
    void init();

    /** Inits board from string (row-major X, O or .) */
    void init(const std::string& str);

    /** Returns side to move, either X or O */
    Piece stm() const { return stm_; }

    /** Changes side to move, returns new value. */
    Piece flipStm();

    /** Parses a string like 'a1' and return the move.
        InvalidMove on error. */
    Move parseMove(const std::string& str) const;

    /** execute move for stm */
    void makeMove(Move m);

    /** pushes all moves onto @p m.
        m will be cleared before. */
    void getMoves(Moves& m) const;

    /** Returns the utility for X or O */
    int eval(PieceType p) const;

    /** print the board as ascii */
    void printBoard(std::ostream& out = std::cout) const;

protected:
    unsigned int size_;
    std::vector<Piece> board_;

    Piece stm_;
};

#endif // BOARD_H
