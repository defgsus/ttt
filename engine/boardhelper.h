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

private:
    /** return the evaluation value of one row */
    int getRowValue_(int * row, int x, int o) const;
    void createRowValues_();
    void createMoveOrder_();
    void createScanOrder_();

    uint size_,
         sizesq_,
         cons_;

    /** evaluation of all row combinations */
    std::vector<int> rowValues_;
    /** indices to fixed order of moves (which square to analyze first) */
    std::vector<uint> moveOrder_;
    /** indices to all possible rows. */
    std::vector<uint> scanOrder_;

    std::vector<uint> flags_;
};


} // namespace TTT

#endif // TTT_ENGINE_BOARDHELPER_H
