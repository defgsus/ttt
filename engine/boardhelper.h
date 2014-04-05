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

#ifndef TTT_ENGINE_BOARDHELPER_H
#define TTT_ENGINE_BOARDHELPER_H

#include "common.h"

namespace TTT {

class Board;

/** Class for generating moves and evaluating a Board */
class BoardHelper
{
public:

    /** Creates an accel. structure for given size. */
    BoardHelper(uint size, uint consecutives);

    /** Creates an accel. structure for size of given Board. */
    BoardHelper(const Board& b);

    /** Resizes the board accel. structure */
    void setSize(uint size = 3, uint consecutives = 3);

    /** Resizes the board accel. structure (if necessary). */
    void setSize(const Board& b);

    // ----- settings ----

    /** Sets the multiplier for evaluation of captured pieces */
    void setCaptureWeight(int w) { captureWeight_ = w; }

    /** Sets the row evaluation scoring method, currently 0 or 1 */
    void setRowMethod(int m);

    // ----- querry ------

    /** Returns board value */
    int eval(const Board& b) const;

    /** Returns the guessed utility for X (+/-).
        Y's utility values is -evalX() */
    int evalX(const Board& b) const;

    /** win or draw? */
    bool isOver(const Board& b) const { return isWin(b, X) || isWin(b, O); }

    /** Returns whether the side to move has reached the win-utility. */
    bool isWin(const Board& b, Stm stm) const;

    /** pushes all moves onto @p m.
        m will be cleared before. */
    void getMoves(const Board& b, Moves& m) const;

    /* pushes all capture moves onto @p m */
//    void getCaptures(Board& b, Moves& m) const;

private:
    /** return the evaluation value of one row */
    int getRowValue_(int * row, int x, int o) const;
    int getRowValue0_(int * row, int x, int o) const;
    int getRowValue1_(int * row, int x, int o) const;
    void createRowValues_();
    void createMoveOrder_();
    void createScanOrder_();

#ifdef TTT_ONLY_CLOSE_VACANT
    void createNeighbourOrder_();
    void countNeighbours_(const Board& b) const;
#endif

    uint size_,
         sizesq_,
         cons_;

    /** evaluation of all row combinations */
    std::vector<int> rowValues_;
    /** indices to fixed order of moves (which square to analyze first) */
    std::vector<uint> moveOrder_;
    /** indices to all possible rows. */
    std::vector<uint> scanOrder_;

#ifdef TTT_ONLY_CLOSE_VACANT
    mutable std::vector<int> neighbours_;
    std::vector<uint> neighbourOrder_;
#endif

    uint randomness_[0x200];
    mutable uint randpointer_;
    static const uint randsize_ = sizeof(randomness_) / sizeof(uint);

    // ---- settings ---

    int captureWeight_;
    int rowMethod_;
};


} // namespace TTT

#endif // TTT_ENGINE_BOARDHELPER_H
