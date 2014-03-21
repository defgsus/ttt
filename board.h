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
#include <cinttypes>

typedef unsigned int        uint;

typedef unsigned char       Piece;
typedef uint                Square;
typedef Square              Move;
typedef std::vector<Move>   Moves;
typedef Piece               Stm;
typedef uint64_t            Hash;

/** That's what goes onto a board square.
 *
 *  alway assumed: first bit is you, second bit is other
 */
enum PieceType
{
    Empty, X = 1, O = 2
};

const Move InvalidMove = -1;
const int MaxScore = 1000;


const char pieceChar[] = { '.', 'X', 'O' };


class Board
{
public:
    Board(uint size = 3, uint consecutives = 3);

    /** resize the board. */
    void setSize(uint size = 3, uint consecutives = 3);

    /** Clears board and sets stm to X */
    void init();

    /** Inits board from string (row-major X, O or .) */
    void init(const std::string& str);

    /** Returns side to move, either 0 or 1 */
    Stm stm() const { return stm_; }

    /** Changes side to move, returns new value. */
    Stm flipStm();

    /** Returns sidelength. */
    uint size() const { return size_; }

    /** Returns number of to-connect pieces */
    uint consecutives() const { return cons_; }

    /** Returns number of pieces on board. */
    uint pieces() const { return pieces_; }

    /** Return piece at given position */
    Piece pieceAt(Square m) const { return board_[m]; }

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

    bool isWin(Stm p) const;
    bool isDraw() const;

    /** Returns a non-unique hash value for the current position and stm. */
    Hash hash() const;

    /** Returns board value */
    int eval();

    /** Returns the utility for 0 or 1 */
    int eval(Stm p) const;

    void clearEvalMap();
    void setEvalMap(Square s, int score);

    /** print the board as ascii */
    void printBoard(bool witheval, std::ostream& out = std::cout) const;

protected:

    void createRowValues();
    void createHashValues();

    uint size_, cons_;
    std::vector<Piece> board_;
    /** evaluation buffer */
    std::vector<int> score_;

    Stm stm_, nstm_;

    uint pieces_;

    static std::vector<int> rowVal_;
    static std::vector<Hash> hashVal_;
};

#endif // BOARD_H
