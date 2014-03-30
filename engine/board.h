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
    Piece pieceAt(Square m) const { return board_[m] & pieceMask; }
    char pieceCharAt(Square m) const { return pieceChar(board_[m]); }
    /** Sets piece flags, nothing else. @p p must be piece bits only! */
    void setPieceAt(Square m, Piece p) { board_[m] &= ~pieceMask; board_[m] |= p; }

    /** Return whatever value is at position @p m */
    Piece valueAt(Square m) const { return board_[m]; }

    /** Returns 1, 2 or 0 */
    int capturedAt(Square m) const { return (board_[m] & captureMask) >> 2; }

    /** Parses a string like 'a1' and return the move.
        InvalidMove on error. */
    Move parseMove(const std::string& str) const;

    /** Returns the alphanum rep */
    std::string toString(Move m) const;

    /** Is this a valid move? */
    bool canMoveTo(Stm stm, Move m) const;

    /** execute move for stm */
    void makeMove(Move m);

    /** Check for capture possibility from position m along direction xi,yi
        and execute */
    bool exeCapture(Move m, int xi, int yi);

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

    uint size_, sizesq_, cons_;

    std::vector<Piece> board_;

    /** evaluation buffer */
    std::vector<int> score_;

    Stm stm_, nstm_;

    uint pieces_, ply_;

};

} // namespace TTT


#endif // TTT_ENGINE_BOARD_H
