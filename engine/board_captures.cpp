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

#ifdef TTT_CAPTURE
bool Board::canCapture(Square m) const
{
    if (num_captures_ < 0)
        getCaptures_();

    return cap_[m] != 0;
}


int Board::numCapturablePieces() const
{
    if (num_captures_ < 0)
        getCaptures_();

    int n = 0;
    for (uint i=0; i<sizesq_; ++i)
    {
        const int cap = cap_[i];
        for (int j=0; j<8; ++j)
            n += (cap >> (j*4)) & 15;
    }

    return n;
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

    cap_[m] = cap;

#undef TTT_GET_CAPTURE
}






void Board::getCaptures_() const
{
    if (size_ < 3) return;

    num_captures_ = 0;
    //uint32_t cap = 0;

#define TTT_GET_CAPTURE(bp__, xi__, yi__, shift__)   \
    if (pieceAt(bp__) == Empty)                      \
    {                                                \
        const int c = getCapture_(bp__, xi__, yi__); \
        num_captures_ += c;                          \
        cap_[bp__] |= c << (shift__);                \
    }
    /*
    XXX there's some bug here
        may be 3% faster ...
    // topleft
    uint i=0;
    cap_[i] = 0;
    TTT_GET_CAPTURE(i,  1,  0, S_Right);
    TTT_GET_CAPTURE(i,  1,  1, S_RightDown);
    TTT_GET_CAPTURE(i,  0,  1, S_Down);
    ++i;
    // top
    for (; i<size_-1; ++i)
    {
        cap_[i] = 0;
        TTT_GET_CAPTURE(i,  1,  0, S_Right);
        TTT_GET_CAPTURE(i,  1,  1, S_RightDown);
        TTT_GET_CAPTURE(i,  0,  1, S_Down);
        TTT_GET_CAPTURE(i, -1,  0, S_Left);
        TTT_GET_CAPTURE(i, -1,  1, S_LeftDown);
    }
    // topright
    cap_[i] = 0;
    TTT_GET_CAPTURE(i, -1,  0, S_Left);
    TTT_GET_CAPTURE(i, -1,  1, S_LeftDown);
    TTT_GET_CAPTURE(i,  0,  1, S_Down);

    for (uint j=1; j<size_-1; ++j, ++i)
    {
        // left
        cap_[i] = 0;
        TTT_GET_CAPTURE(i,  0, -1, S_Up);
        TTT_GET_CAPTURE(i,  1, -1, S_RightUp);
        TTT_GET_CAPTURE(i,  1,  0, S_Right);
        TTT_GET_CAPTURE(i,  1,  1, S_RightDown);
        TTT_GET_CAPTURE(i,  0,  1, S_Down);
        ++i;
        // middle
        for (uint k=1; k<size_-1; ++k, ++i)
        {
            cap_[i] = 0;
            TTT_GET_CAPTURE(i, -1, -1, S_LeftUp);
            TTT_GET_CAPTURE(i,  0, -1, S_Up);
            TTT_GET_CAPTURE(i,  1, -1, S_RightUp);
            TTT_GET_CAPTURE(i, -1,  0, S_Left);
            TTT_GET_CAPTURE(i,  1,  0, S_Right);
            TTT_GET_CAPTURE(i,  1,  1, S_RightDown);
            TTT_GET_CAPTURE(i,  0,  1, S_Down);
            TTT_GET_CAPTURE(i, -1,  1, S_LeftDown);
        }
        // right
        cap_[i] = 0;
        TTT_GET_CAPTURE(i,  0, -1, S_Up);
        TTT_GET_CAPTURE(i, -1, -1, S_LeftUp);
        TTT_GET_CAPTURE(i, -1,  0, S_Left);
        TTT_GET_CAPTURE(i, -1,  1, S_LeftDown);
        TTT_GET_CAPTURE(i,  0,  1, S_Down);
    }

    // bottomleft
    uint k = sizesq_-size_;
    cap_[k] = 0;
    TTT_GET_CAPTURE(k,  0, -1, S_Up);
    TTT_GET_CAPTURE(k,  1, -1, S_RightUp);
    TTT_GET_CAPTURE(k,  1,  0, S_Right);
    ++k;
    // bottom
    for (uint j=1; j<size_-1; ++j, ++k)
    {
        cap_[k] = 0;
        TTT_GET_CAPTURE(k,  1,  0, S_Right);
        TTT_GET_CAPTURE(k,  1,  1, S_RightUp);
        TTT_GET_CAPTURE(k,  0,  1, S_Up);
        TTT_GET_CAPTURE(k, -1,  1, S_LeftUp);
        TTT_GET_CAPTURE(k, -1,  0, S_Left);
    }
    // bottomright
    cap_[k] = 0;
    TTT_GET_CAPTURE(k,  0, -1, S_Up);
    TTT_GET_CAPTURE(k, -1, -1, S_LeftUp);
    TTT_GET_CAPTURE(k, -1,  0, S_Left);
    */

    for (uint i=0; i<sizesq_; ++i)
    {
        cap_[i] = 0;
        TTT_GET_CAPTURE(i, -1, -1, S_LeftUp);
        TTT_GET_CAPTURE(i,  0, -1, S_Up);
        TTT_GET_CAPTURE(i,  1, -1, S_RightUp);
        TTT_GET_CAPTURE(i, -1,  0, S_Left);
        TTT_GET_CAPTURE(i,  1,  0, S_Right);
        TTT_GET_CAPTURE(i,  1,  1, S_RightDown);
        TTT_GET_CAPTURE(i,  0,  1, S_Down);
        TTT_GET_CAPTURE(i, -1,  1, S_LeftDown);
    }

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
    const int cap = cap_[m];

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
            board_[pos] = 8;                       \
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
                    board_[pos] = 8;
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


#endif // TTT_CAPTURE

} // namespace TTT
