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

#include "boardhelper.h"
#include "board.h"

#include <math.h>
#include <iomanip>

#define TTT_BOARD_CHECK \
    TTT_ASSERT(b.size_ == size_ && b.cons_ == cons_, \
            "BoardHelper ("<<size_<<", "<<cons_<<") & Board ("<<b.size_<<", "<<b.cons_<<") not matching")


namespace TTT {



BoardHelper::BoardHelper(uint size, uint consecutives)
    :   size_(size),
        cons_(consecutives)
{
    createRowValues_();
    createMoveOrder_();
    createScanOrder_();
}

BoardHelper::BoardHelper(const Board& b)
    :   size_(b.size_),
        cons_(b.cons_)
{
    createRowValues_();
    createMoveOrder_();
    createScanOrder_();
}

void BoardHelper::setSize(uint size, uint consecutives)
{
    size_ = size;
    cons_ = consecutives;
    createRowValues_();
    createMoveOrder_();
    createScanOrder_();
}

void BoardHelper::setSize(const Board &b)
{
    if (b.size_ != size_ || b.cons_ != cons_)
        setSize(b.size_, b.cons_);
}

void BoardHelper::createMoveOrder_()
{
    moveOrder_.resize(size_ * size_);

    for (auto &i : moveOrder_)
        i = 0;

    // use an ulam spiral to create indexes
    // that start in the middle and expand towards edges
    const int sh = (size_ >> 1) - !(size_&1);
    for (int y = 0; y<(int)size_; ++y)
    for (int x = 0; x<(int)size_; ++x)
    {
        size_t u = ulam_spiral(x-sh, y-sh);

        if (u < moveOrder_.size())
            moveOrder_[u] = (y * size_ + x);
    }

    //for (auto i : moveOrder_)
    //    std::cout << " " << i;
}

int BoardHelper::getRowValue_(int * row, const int X, const int O) const
{
    // get utility value
    int u = 0, full = 0;

    // start by scoring
    //   square values as:
    for (uint i = 0; i<cons_; ++i)
    {
        if (row[i] == Empty)
            u += 1;
        else if (row[i] == X)
        {
            full++;
            u += (4 + 2 * full);
        }
        else if (row[i] == O)
            u -= 3;
    }
    // full row?
    if (full>=(int)cons_)
    {
        u = MaxScore; // win
    }
    else
    {
        // additional points for consecutiveness
        for (uint i=0; i<cons_-1; ++i)
            if (row[i] == X && row[i+1] == X)
                u *= 2;

        // generally less points when opponent breaks row
        for (uint i = 0; i<cons_; ++i)
            if (row[i] == O) { u /= 3; break; }
#if 1
        // no points for full-row-emptyness
        if (full == 0) u = 0;
#endif

    }

    return std::max(0,u);
}


void BoardHelper::createRowValues_()
{
    rowValues_.clear();

    // creat all permutations of empty/self/other for a row of length cons
    // and precalculate the utility value
    // Note.
    // The value '3' in the 2 piece bits is not used.
    // The map still contains a value there for efficient access
    // (entry = rowvalue<<2)
    uint i,j,kk,k = 0, num = pow(4, cons_);
    for (j=0; j<num; ++j, ++k)
    {
        std::vector<int> row;
        for (i=0; i<cons_; ++i)
        {
            kk = (k>>(i*2)) & pieceMask;
            if ((kk)==Empty) row.push_back(Empty); else
            if ((kk)==X)     row.push_back(X); else
            if ((kk)>=O)     row.push_back(O);
        }

        int u = getRowValue_(&row[0], X, O)
              - getRowValue_(&row[0], O, X);

        // store utility value
        rowValues_.push_back( u );

#if 0
        // debug print
        if (k%4 != 3)
        {
            std::cout << std::setw(4) << k << " ";
            for (i=0; i<cons_; ++i) std::cout << pieceChar(row[i]);
            if (u!=0) std::cout << " " << u;
            std::cout << std::endl;
        }
#endif
    }
}


void BoardHelper::createScanOrder_()
{
    scanOrder_.clear();

#define TTT_SCAN(xi, yi)                       \
    {                                          \
        uint cx = x, cy = y;                   \
        for (uint i=0; i<cons_; ++i)           \
        {                                      \
    /*TTT_DEBUG("x="<<cx<<", y="<<cy);*/ \
            scanOrder_.push_back(cy*size_+cx); \
            cx += (xi); cy += (yi);            \
        }                                      \
    }

    uint x,y;

    // horizontal
    for (y=0; y<size_; ++y)
        for (x=0; x<=size_-cons_; ++x)
            TTT_SCAN(1,0);

    // vertically
    for (x=0; x<size_; ++x)
        for (y=0; y<=size_-cons_; ++y)
            TTT_SCAN(0,1);

    // diagonally right
    for (y=0; y<=size_-cons_; ++y)
        for (x=0; x<=size_-cons_; ++x)
            TTT_SCAN(1,1);

    // diagonally left
    for (y=0; y<=size_-cons_; ++y)
        for (x=size_-1; x>=cons_-1; --x)
            TTT_SCAN(-1,1);

    #undef TTT_SCAN

    //TTT_DEBUG(scanOrder.size());
}



// -------------------- query ---------------------


int BoardHelper::eval(const Board& b) const
{
    TTT_BOARD_CHECK;

    return (b.stm_==X)? evalX(b) : -evalX(b);
}

int BoardHelper::evalX(const Board& b) const
{
    TTT_BOARD_CHECK;

    int u = 0;

    // --- find consecutives ---

    for (uint i=0; i<scanOrder_.size(); )
    {
        // bit-fiddle row value
        // which is an index into rowVal_[]
        uint cnt = 0;
        for (uint j=0; j<cons_; ++j, ++i)
        {
            cnt <<= 2;
            cnt += b.pieceAt(scanOrder_[i]);
        }
        // add evaluation value
        u += rowValues_[cnt];
    }

    return u;
}

bool BoardHelper::isWin(const Board& b, Stm p) const
{
    TTT_BOARD_CHECK;

    int e = evalX(b);
    return ((p == X && e >= WinScore)
            || (p == O && -e >= WinScore));
}


void BoardHelper::getMoves(const Board& b, Moves &m) const
{
    TTT_BOARD_CHECK;

    m.clear();
    for (size_t i=0; i<moveOrder_.size(); ++i)
    {
        const Square k = moveOrder_[i];

        if (b.canMoveTo(b.stm_, k))
            m.push_back(k);
    }
}


#undef TTT_BOARD_CHECK

} // namespace TTT



