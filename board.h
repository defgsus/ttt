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

#define TTT_CAPTURE
//#define TTT_KEEP_TREE

#include <vector>
#include <string>
#include <iostream>

typedef unsigned char Piece;
typedef unsigned int Move;
typedef std::vector<Move> Moves;

const Move InvalidMove = -1;
const int MaxScore = 1000;

enum PieceType
{
    Empty, X, O
};

const char pieceChar[] = { '.', 'X', 'O' };

class Board
{
public:
    Board(unsigned int size = 3, unsigned int consecutives = 3);

    /** resize the board. */
    void setSize(unsigned int size = 3, unsigned int consecutives = 3);

    /** Clears board and sets stm to X */
    void init();

    /** Inits board from string (row-major X, O or .) */
    void init(const std::string& str);

    /** Returns side to move, either X or O */
    Piece stm() const { return stm_; }

    /** Changes side to move, returns new value. */
    Piece flipStm();

    /** Returns sidelength. */
    unsigned int size() const { return size_; }
    /** Returns number of to-connect pieces */
    unsigned int consecutives() const { return cons_; }

    /** Returns number of pieces on board. */
    unsigned int pieces() const { return pieces_; }

    /** Return piece at given position */
    Piece pieceAt(Move m) const { return board_[m]; }

    /** Parses a string like 'a1' and return the move.
        InvalidMove on error. */
    Move parseMove(const std::string& str) const;

    /** Returns the alphanum rep */
    std::string toString(Move m) const;

    /** execute move for stm */
    void makeMove(Move m);

    /** pushes all moves onto @p m.
        m will be cleared before. */
    void getMoves(Moves& m) const;

    /** Check for capture possibility from position m along direction xi,yi */
    bool canCapture(Move m, int xi, int yi) const;

    /** Returns board value */
    int eval();

    /** Returns the utility for X or O */
    int eval(PieceType p) const;

    void clearEvalMap();
    void setEvalMap(int Move, int score);

    /** print the board as ascii */
    void printBoard(bool witheval, std::ostream& out = std::cout) const;

protected:

    void createRowValues();

    unsigned int size_, cons_;
    std::vector<Piece> board_;
    std::vector<int> score_;

    Piece stm_, nstm_;
    unsigned int pieces_;

    static std::vector<int> rowVal_;
};

#endif // BOARD_H
