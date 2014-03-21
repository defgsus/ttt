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
#define TTT_KEEP_TREE
//#define TTT_TRANSPOSITION_TABLE

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


/** That's what goes onto a board square.
 *
 *  alway assumed: first bit is you, second bit is other
 */
enum PieceType
{
    Empty, X = 1, O = 2
};
const char pieceChar[] = { '.', 'X', 'O' };

const Move InvalidMove = -1;
const int MaxScore = 1000;

#ifdef TTT_TRANSPOSITION_TABLE
struct Hash
{
    typedef uint64_t Type;
    Type a, b;

    static void getHash(Hash& h, const Piece * board, uint s, Stm stm)
    {
        h.a = h.b = 0;
        Type * a = &h.a;
        for (uint i=0; i<s; ++i, ++board)
        {
            if (i == 32) a = &h.b;
            *a |= *board;
            *a <<= 2;
        }
        h.b |= ((Type)stm) << 62;
    }

    bool operator == (const Hash& r) const
    {
        return r.a == a && r.b == b;
    }

    bool operator < (const Hash& r) const
    {
        // XXX
        return (a < r.a) || (b < r.b);
    }

};
#endif


/** good ol' implicit ulam spiral for move ordering
 *  [starting at zero] */
template <typename I>
static I ulam_spiral(I x, I y)
{
    I d;
    if ((d= x)>abs(y)) return (d * 4 - 3) * d + y;
    if ((d=-x)>abs(y)) return (d * 4 + 1) * d - y;
    {
        d = abs(y);
        if (y > 0) return (d * 4 - 1) * d - x;
              else return (d * 4 + 3) * d + x;
    }
}


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

#ifdef TTT_TRANSPOSITION_TABLE
    /** Returns a non-unique hash value for the current position and stm. */
    void getHash(Hash& h) const { return Hash::getHash(h, &board_[0], size_*size_, stm_); }
#endif

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
    void createMoveOrder();

    uint size_, cons_;
    std::vector<Piece> board_;
    /** evaluation buffer */
    std::vector<int> score_;

    Stm stm_, nstm_;

    uint pieces_, ply_;

    static std::vector<int> rowVal_;
    static std::vector<Square> moveOrder_;
};

#endif // BOARD_H
