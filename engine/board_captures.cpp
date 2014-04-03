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

#include "board.h"

namespace TTT {

int Board::numCapturablePieces() const
{
    if (num_captures_ < 0)
        getCaptures_();

    int n = 0;
    for (uint i=0; i<sizesq_; ++i)
    {
        const int cap = board_[i].cap;
        for (int j=0; j<8; ++j)
            n += (cap >> (j*4)) & 15;
    }

    return n;
}

bool Board::canCapture(Square m) const
{
    if (num_captures_ < 0)
        getCaptures_();

    return board_[m].cap != 0;
}


void Board::getCaptures_() const
{
    num_captures_ = 0;
    uint32_t cap = 0;

#define TTT_GET_CAPTURE(bp__, xi__, yi__, shift__)   \
    if (pieceAt(bp__) == Empty)                      \
    {                                                \
        const int c = getCapture_(bp__, xi__, yi__); \
        num_captures_ += c;                          \
        cap |= c << (shift__);                       \
    }

    for (uint i=0; i<sizesq_; ++i)
    {
        cap = 0;

        TTT_GET_CAPTURE(i, -1, -1, S_LeftUp);
        TTT_GET_CAPTURE(i,  0, -1, S_Up);
        TTT_GET_CAPTURE(i,  1, -1, S_RightUp);
        TTT_GET_CAPTURE(i, -1,  0, S_Left);
        TTT_GET_CAPTURE(i,  1,  0, S_Right);
        TTT_GET_CAPTURE(i,  1,  1, S_RightDown);
        TTT_GET_CAPTURE(i,  0,  1, S_Down);
        TTT_GET_CAPTURE(i, -1,  1, S_LeftDown);

        board_[i].cap = cap;
    }
#undef TTT_GET_CAPTURE
}

void Board::getCapture_(Square m) const
{
#define TTT_GET_CAPTURE(bp__, xi__, yi__, shift__)   \
    if (pieceAt(bp__) == Empty)                      \
    {                                                \
        const int c = getCapture_(bp__, xi__, yi__); \
        cap |= c << (shift__);                       \
    }

    int cap = 0;

    TTT_GET_CAPTURE(m, -1, -1, S_LeftUp);
    TTT_GET_CAPTURE(m,  0, -1, S_Up);
    TTT_GET_CAPTURE(m,  1, -1, S_RightUp);
    TTT_GET_CAPTURE(m, -1,  0, S_Left);
    TTT_GET_CAPTURE(m,  1,  0, S_Right);
    TTT_GET_CAPTURE(m,  1,  1, S_RightDown);
    TTT_GET_CAPTURE(m,  0,  1, S_Down);
    TTT_GET_CAPTURE(m, -1,  1, S_LeftDown);

    board_[m].cap = cap;

#undef TTT_GET_CAPTURE
}

int Board::getCapture_(Square m, int xi, int yi) const
{
    int pos = m, px = m%size_;
    const int inc = xi + yi * size_;

    // opponent count
    int op = 0;

    for (int i=0; i<(int)size_; ++i)
    {
        pos += inc;
        px += xi;

        // outside board
        if (px < 0 || px >= (int)size_ ||
            pos < 0 || pos >= (int)sizesq_)
            return 0;

        const Piece piece = pieceAt(pos);
        if (piece == Empty)
            return 0;
        else
        if (piece == nstm_)
            ++op;
        else
        if (piece == stm_)
        {
            return op;
        }
    }
    return 0;
}



int Board::exeCapture_(Square m)
{
    const int cap = board_[m].cap;

    int count = 0;

#define TTT_EXE_CAPTURE(bp__, xi__, yi__, shift__)   \
    {                                                \
        const int inc = (xi__) + (int)size_ * (yi__);\
        const int cnt = (cap >> (shift__)) & 15;     \
        int pos = bp__;                              \
        for (int i=0; i<cnt; ++i)                    \
        {                                            \
            pos += inc;                              \
            /* set empty + flags */                  \
            board_[pos].v = 8;                       \
            pieces_--;                               \
        }                                            \
        count += cnt;                                \
    }

    TTT_EXE_CAPTURE(m, -1, -1, S_LeftUp);
    TTT_EXE_CAPTURE(m,  0, -1, S_Up);
    TTT_EXE_CAPTURE(m,  1, -1, S_RightUp);
    TTT_EXE_CAPTURE(m, -1,  0, S_Left);
    TTT_EXE_CAPTURE(m,  1,  0, S_Right);
    TTT_EXE_CAPTURE(m,  1,  1, S_RightDown);
    TTT_EXE_CAPTURE(m,  0,  1, S_Down);
    TTT_EXE_CAPTURE(m, -1,  1, S_LeftDown);

#undef TTT_EXE_CAPTURE

    return count;
}





#ifdef THATS_THE_OLD_VERSION
bool Board::exeCapture(Square m, int xi, int yi)
{
    if (pieceAt(m) != stm_) return false;

    int pos = m, px = m%size_;
    const int inc = xi + yi * size_;

    // opponent count
    int op = 0;

    for (int i=0; i<(int)size_; ++i)
    {
        pos += inc;
        px += xi;

        if (px < 0 || px >= (int)size_ ||
            pos < 0 || pos >= (int)sizesq_)
            return false;

        const Piece piece = pieceAt(pos);
        if (piece == Empty)
            return false;
        else
        if (piece == nstm_)
            ++op;
        else
        if (piece == stm_)
        {
            if (op>0)
            {
                // remove pieces
                for (int j=0; j<op; ++j)
                {
                    pos -= inc;
                    // set empty + flags
                    board_[pos].v = 8;
                }
                pieces_ -= op;
                return true;
            }
            return false;
        }
    }
    return false;
}
#endif



} // namespace TTT
