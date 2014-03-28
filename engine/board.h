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

// --------- compile settings --------------

/** Multi-threading support (fixed to 8 in Search::bestMove()) */
#define TTT_THREADED
/** Allow captures */
#define TTT_CAPTURE
/** Force one move pause for captured squares */
#define TTT_CAPTURE_WAIT
/** Use Alpha-Beta pruning */
#define TTT_ALPHA_BETA
/** Use a greedyness value to cut-off nodes */
#define TTT_GREEDY
/** Use a hash table (worse performance in threading mode */
//#define TTT_TRANSPOSITION_TABLE
/** Keep the entire search tree (for display). */
//#define TTT_KEEP_TREE

#include <vector>
#include <string>
#include <iostream>
#include <cinttypes>

namespace TTT {


typedef unsigned int        uint;

typedef unsigned char       Piece;
typedef uint                Square;
typedef Square              Move;
typedef std::vector<Move>   Moves;
typedef Piece               Stm;


/** That's what goes onto a board square.
 *
 *  always assumed: first bit is you, second bit is other
 */
enum PieceType
{
    Empty, X = 1, O = 2, Defunkt = 4
};

const Move InvalidMove = -1;
const int MaxScore = 7000;
const int WinScore = MaxScore / 2;
const int InvalidScore = MaxScore*10;
const Piece pieceMask = 3;
const Piece captureMask = 4+8;

inline char pieceChar(Piece p)
{
    return p == 0 ?
        '.' : (p&pieceMask) != 0 ?
                (p&X ? 'X' : 'O') : '~';
}

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
            *a |= *board & pieceMask;
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
        return (b == r.b)? (a < r.a) : (b < r.b);
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

class BoardHelper
{
public:

    BoardHelper(uint size, uint consecutives)
        :   size(size), cons(consecutives)
    {
        createRowValues();
        createMoveOrder();
        createScanOrder();
    }

    /** return the evaluation value of one row */
    int getRowValue(int * row, int x, int o) const;
    void createRowValues();
    void createMoveOrder();
    void createScanOrder();

    uint size,
         cons;

    /** evaluation of all row combinations */
    std::vector<int> rowValues;
    /** indices to fixed order of moves (which square to analyze first) */
    std::vector<uint> moveOrder;
    /** indices to all possible rows. */
    std::vector<uint> scanOrder;

};



class Board
{
public:
    Board(uint size = 3, uint consecutives = 3);
    ~Board();

    Board(const Board& rhs);

    Board& operator = (const Board& rhs);

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
    Piece pieceAt(Square m) const { return board_[m] & pieceMask; }
    char pieceCharAt(Square m) const { return pieceChar(board_[m]); }
    /** Sets piece flags, nothing else. @p p must be piece bits only! */
    void setPieceAt(Square m, Piece p) { board_[m] &= ~pieceMask; board_[m] |= p; }

    bool isCaptured(Square m) const { return board_[m] & captureMask; }

    /** Is this a value move? */
    bool canMoveTo(Stm stm, Move m) const;

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

    /** Check for capture possibility from position m along direction xi,yi
        and execute */
    bool exeCapture(Move m, int xi, int yi);

    /** win or draw? */
    bool isOver() const { return isWin(X) || isWin(O); }
    /** Returns whether the side to move has reached the win-utility. */
    bool isWin(Stm stm) const;
    bool isDraw() const;

#ifdef TTT_TRANSPOSITION_TABLE
    /** Returns a non-unique hash value for the current position and stm. */
    void getHash(Hash& h) const { return Hash::getHash(h, &board_[0], size_*size_, stm_); }
#endif

    /** Returns board value */
    int eval();

    /** Returns the guessed utility for X (+/-).
        Y's utility values is -evalX() */
    int evalX() const;

    void clearEvalMap();
    void setEvalMap(Square s, int score);

    /** print the board as ascii */
    void printBoard(bool witheval, std::ostream& out = std::cout) const;

protected:

    BoardHelper * helper_;
    bool ownHelper_;

    uint size_, sizesq_, cons_;

    std::vector<Piece> board_;

    /** evaluation buffer */
    std::vector<int> score_;

    Stm stm_, nstm_;

    uint pieces_, ply_;

};

} // namespace TTT


#endif // BOARD_H
