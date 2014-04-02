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

#ifndef TTT_ENGINE_BOARD_H
#define TTT_ENGINE_BOARD_H

#include "common.h"


namespace TTT {


/** Represents a position, side-to-move and such */
class Board
{
    friend class BoardHelper;

public:

    enum DirectionShifts
    {
        S_LeftUp    =  0,
        S_Up        =  4,
        S_RightUp   =  8,
        S_Left      = 12,
        S_Right     = 16,
        S_RightDown = 20,
        S_Down      = 24,
        S_LeftDown  = 28
    };

    struct BoardData
    {
        uint32_t v;

        /** 4 bits of length for each direction, left-up, up, up-right, right ... */
        mutable uint32_t cap;
    };

    static const Piece pieceMask = 3;
    static const Piece captureLockedMask = 4+8;


#if 0
    enum FlagMask
    {
        /** current max capture length ! */
        M_CaptureLength = 0xff,
        /** DirectionFlags */
        M_CaptureDir = 0xff00

    };
#endif


    /** Creates empty board with white-to-move */
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

    static char pieceChar(Piece p);

    /** Parses a string like 'a1' and return the move.
        InvalidMove on error. */
    Move parseMove(const std::string& str) const;

    /** Returns the alphanum rep */
    std::string toString(Move m) const;

    /** Return piece at given position */
    Piece pieceAt(Square m) const { return board_[m].v & pieceMask; }
    char pieceCharAt(Square m) const { return pieceChar(board_[m].v); }
    /** Sets piece flags, nothing else. @p p must be piece bits only! */
    void setPieceAt(Square m, Piece p) { board_[m].v &= ~pieceMask; board_[m].v |= p; }

    /** Return whatever value is at position @p m */
    Piece valueAt(Square m) const { return board_[m].v; }

    /** Returns 1, 2 or 0 */
    int blockedAt(Square m) const { return (board_[m].v & captureLockedMask) >> 2; }

    /** Is this a valid move? */
    bool canMoveTo(Stm stm, Move m) const;

    /** execute move for stm */
    void makeMove(Move m);

    /** Check for capture possibility from position m along direction xi,yi
        and execute */
    //bool exeCapture(Move m, int xi, int yi);

    /** Number of possible captures for stm */
    int numCaptures() const { if (num_captures_<0) getCaptures_(); return num_captures_; }

    int numCapturablePieces() const;

    //int canBeCaptured()

    /** Number of pieces >= size*size ? */
    bool isDraw() const;

#ifdef TTT_TRANSPOSITION_TABLE
    /** Returns a non-unique hash value for the current position and stm. */
    void getHash(Hash& h) const { return Hash::getHash(h, &board_[0], size_*size_, stm_); }
#endif

    void clearEvalMap();
    void setEvalMap(Square s, int score);
    void copyEvalFrom(const Board& b);

    /** print the board as ascii */
    void printBoard(bool witheval, std::ostream& out = std::cout) const;

protected:

    /** Updates one field: board_[m].cap */
    void getCapture_(Square m) const;

    void getCaptures_() const;
    int getCapture_(Square m, int xi, int yi) const;
    void exeCapture_(Square m);

    uint size_, sizesq_, cons_;

    std::vector<BoardData> board_;

    /** evaluation buffer */
    std::vector<int> score_;

    // ----- running variables -----

    Stm stm_, nstm_;

    uint pieces_, ply_;
    /** if -1, not initialized */
    mutable int  num_captures_;

};



inline char Board::pieceChar(Piece p)
{
    return p == 0 ?
        '.' : (p&pieceMask) != 0 ?
                ((p&X) ? 'X' : 'O') : (((p&captureLockedMask)==8)? '2' : '1');
}

} // namespace TTT


#endif // TTT_ENGINE_BOARD_H
