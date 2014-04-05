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
#include <stdlib.h>
#include <iomanip>

#define TTT_BOARD_CHECK \
    TTT_ASSERT(b.size_ == size_ && b.cons_ == cons_, \
            "BoardHelper ("<<size_<<", "<<cons_<<") & Board ("<<b.size_<<", "<<b.cons_<<") not matching")


namespace TTT {



BoardHelper::BoardHelper(uint size, uint consecutives)
    :   size_(size),
        cons_(consecutives),
        captureWeight_(0),
        rowMethod_(0)
{
    setSize(size_, cons_);
}

BoardHelper::BoardHelper(const Board& b)
    :   size_(b.size_),
        cons_(b.cons_),
        captureWeight_(0)
{
    setSize(size_, cons_);
}

void BoardHelper::setSize(uint size, uint consecutives)
{
    size_ = size;
    cons_ = consecutives;
    sizesq_ = size_ * size_;

    createRowValues_();
    createMoveOrder_();
    createScanOrder_();
#ifdef TTT_CAPTURE_EVALUATE
    createNeighbourOrder_();
#endif

    randpointer_ = rand() & (randsize_-1);
    for (uint i=0; i<randsize_; ++i)
        randomness_[i] = rand();

    /*
    flags_.resize(sizesq_);
    for (auto &i : flags_)
        i = 0;
    */
}

void BoardHelper::setSize(const Board &b)
{
    if (b.size_ != size_ || b.cons_ != cons_)
        setSize(b.size_, b.cons_);
}

void BoardHelper::setRowMethod(int m)
{
    if (rowMethod_ == m)
        return;

    rowMethod_ = m;
    createRowValues_();
}

void BoardHelper::createMoveOrder_()
{
    moveOrder_.resize(sizesq_);

    for (auto &i : moveOrder_)
        i = 0;

    // use an ulam spiral to create indices
    // that start in the middle and expand towards edges
    const int sh = (size_ >> 1) - !(size_&1);
    for (int y = 0; y<(int)size_; ++y)
    for (int x = 0; x<(int)size_; ++x)
    {
        size_t u = ulam_spiral(x-sh, y-sh);

        if (u < moveOrder_.size())
            moveOrder_[u] = (y * size_ + x);
    }

    // revert (to verify that above is a good basic ordering,
    // at least for current evaluation function ;)
    //for (size_t i = 0; i < moveOrder_.size()/2; ++i)
    //    std::swap(moveOrder_[i], moveOrder_[moveOrder_.size()-1-i]);
}

int BoardHelper::getRowValue_(int * row, const int X, const int O) const
{
    switch (rowMethod_)
    {
        default: return getRowValue0_(row, X, O);
        case 1: return getRowValue1_(row, X, O);
    }
}

int BoardHelper::getRowValue0_(int * row, const int X, const int O) const
{
    // -- get probable utility value --

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
#if (1)
        // no points for full-row-emptyness
        if (full == 0) u = 0;
#endif

        // make relative to length (not working right)
        //u = (u) / std::max(1u, sizesq_ / 5);
    }

#if (0)
    // limit score to draw/win
    // (disable tactics)
    if (u<WinScore) u = 0;
    u = std::min(WinScore, u);
#endif

    return std::max(0,u);
}

int BoardHelper::getRowValue1_(int * row, const int X, const int O) const
{
    // -- get probable utility value --

    int u = 0, own = 0, other = 0;

    // count each
    for (uint i = 0; i<cons_; ++i)
    {
        if (row[i] == X)
            own++;
        else
        if (row[i] == O)
            other++;
    }

    // full row?
    if (own == (int)cons_)
    {
        return MaxScore; // win
    }

    u = own;

    // additional points for consecutiveness
    for (uint i=0; i<cons_-1; ++i)
        if (row[i] == X && row[i+1] == X)
            u *= 2;

    if (own == (int)cons_-1)
        u *= 10;

    // generally less points when opponent breaks row
    u /= (1 + 2*other);

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
            kk = (k>>(i*2)) & Board::pieceMask;
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
            for (i=0; i<cons_; ++i) std::cout << Board::pieceChar(row[i]);
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

    int e = evalX(b);

    return (b.stm_==X)? e : -e;
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

#ifdef TTT_CAPTURE_EVALUATE
    u += captureWeight_
            * (  b.numAllCaptured(X)
               - b.numAllCaptured(O) );
#endif

    return u;
}

#ifdef TTT_CAPTURE_EVALUATE

void BoardHelper::createNeighbourOrder_()
{
    neighbours_.resize(sizesq_+1);

    neighbourOrder_.clear();

#define TTT_ADD_N(y, x)                                          \
    if ((x)>=0 && (x)<(int)size_ && (y)>=0 && (y)<(int)size_)   \
        neighbourOrder_.push_back((y)*size_+(x));               \
    else                                                        \
        neighbourOrder_.push_back(size_);

    for (int j=0; j<(int)size_; ++j)
    for (int i=0; i<(int)size_; ++i)
    {
        TTT_ADD_N(j-1, i-1)
        TTT_ADD_N(j-1, i  )
        TTT_ADD_N(j-1, i+1)
        TTT_ADD_N(j  , i-1)
        TTT_ADD_N(j  , i+1)
        TTT_ADD_N(j+1, i-1)
        TTT_ADD_N(j+1, i  )
        TTT_ADD_N(j+1, i+1)
    }

#undef TTT_ADD_N
}

void BoardHelper::countNeighbours_(const Board& b) const
{
    for (auto &i : neighbours_)
        i = 0;

    auto n = neighbourOrder_.begin();
    for (size_t i=0; i<sizesq_; ++i)
    {
        if (b.pieceAt(i) == Empty)
        {
            n+=8;
            continue;
        }

        for (size_t j=0; j<8; ++j)
            neighbours_[*n++]++;
    }
}
#endif

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

#ifdef TTT_ONLY_CLOSE_VACANT
    countNeighbours_(b);
#endif

    // check captures first
    for (size_t i=0; i<moveOrder_.size(); ++i)
    {
        const Square k = moveOrder_[i];

        if (
#ifdef TTT_ONLY_CLOSE_VACANT
                neighbours_[k] &&
#endif
                b.canMoveTo(b.stm_, k) && b.canCapture(k)
            )
            m.push_back(k);
    }

    // non-captures
    for (size_t i=0; i<moveOrder_.size(); ++i)
    {
        const Square k = moveOrder_[i];

        if (
#ifdef TTT_ONLY_CLOSE_VACANT
                neighbours_[k] &&
#endif
                b.canMoveTo(b.stm_, k) && !b.canCapture(k)
            )
            m.push_back(k);
    }

#ifdef TTT_RANDOMNESS
    if (!m.empty())
    for (size_t i=0; i<size_; ++i)
    {
        const uint
            f = (randomness_[(randpointer_++) & (randsize_-1)] ) % m.size(),
            t = (randomness_[(randpointer_++) & (randsize_-1)] ) % m.size();
        std::swap(m[f], m[t]);
    }
#endif

}


#undef TTT_BOARD_CHECK

} // namespace TTT



