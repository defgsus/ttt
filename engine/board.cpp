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

#include <iomanip>


#define BOARD_INC_UP         ((int)(-size_))
#define BOARD_INC_LEFT_UP    ((int)(-size_ - 1))
#define BOARD_INC_RIGHT_UP   ((int)(-size_ + 1))
#define BOARD_INC_LEFT       ((int)-1)
#define BOARD_INC_RIGHT      ((int)1)
#define BOARD_INC_LEFT_DOWN  ((int)(size_-1))
#define BOARD_INC_DOWN       ((int)(size_))
#define BOARD_INC_RIGHT_DOWN ((int)(size_+1))


namespace TTT {


// -------------------------- board ---------------------------

Board::Board(uint size, uint cons)
    :   size_   (std::min(maxBoardSize, size)),
        sizesq_ (size_ * size_),
        cons_   (cons),
        board_  (sizesq_),
        score_  (sizesq_)
{
    TTT_DEBUG("Board::Board(" << size << ", " << cons << ")");
    init();
}

void Board::init()
{
    for (auto &i : board_)
        i.v = Empty;
    clearEvalMap();

    stm_ = X;
    nstm_ = O;
    pieces_ = 0;
    ply_ = 0;

    num_captures_ = -1;
}

void Board::setSize(uint size, uint cons)
{
    size_ = std::max(2u, std::min(maxBoardSize, size ));
    sizesq_ = size_ * size_;
    cons_ = std::min(cons, size_);
    board_.resize(sizesq_);
    score_.resize(sizesq_);

    init();
}


Stm Board::flipStm()
{
    ply_++;
    std::swap(stm_, nstm_);
    return stm_;
}


void Board::init(const std::string& str)
{
    init();
    for (size_t i=0; i<str.length() && i<board_.size(); ++i)
    {
        Piece p = Empty;
        switch (str[i])
        {
            case 'X': p = X; pieces_++; break;
            case 'O': p = O; pieces_++; break;
            case 'x': p = 4; break;
            case 'o': p = 8; break;
        }
        board_[i].v = p;
    }
}


Move Board::parseMove(const std::string& str) const
{
    if (str.size() < 2)
        return InvalidMove;

    uint x, y;

    x = str[0];
    if (x>='A' && x<='Z')
        x -= 'A';
    else
    if (x>='a' && x<='z')
        x -= 'a';
    else
        return InvalidMove;

    y = str[1];
    if (y>='1' && y<='9')
        y -= '1';
    else
        return InvalidMove;

    if (x>=size_ || y>=size_)
        return InvalidMove;

    x = y * size_ + x;

    if (!canMoveTo(stm_, x))
        return InvalidMove;

    return x;
}

bool Board::canMoveTo(Stm /*stm*/, Move m) const
{
    return
           m < sizesq_
        && pieceAt(m) == Empty
#ifdef TTT_CAPTURE_WAIT
        && !blockedAt(m)
#endif
    ;
}

void Board::getCaptures_()
{
    num_captures_ = 0;
    uint32_t cap = 0;

#define TTT_EXE_CAPTURE(bp__, xi__, yi__, shift__)   \
    if (pieceAt(bp__) == Empty)                      \
    {                                                \
        const int c = getCapture_(bp__, xi__, yi__); \
        num_captures_ += c;                          \
        cap |= c << (shift__);                       \
    }

    for (uint i=0; i<sizesq_; ++i)
    {
        TTT_EXE_CAPTURE(i, -1, -1, S_LeftUp);
        TTT_EXE_CAPTURE(i,  0, -1, S_Up);
        TTT_EXE_CAPTURE(i,  1, -1, S_RightUp);
        TTT_EXE_CAPTURE(i, -1,  0, S_Left);
        TTT_EXE_CAPTURE(i,  1,  0, S_Right);
        TTT_EXE_CAPTURE(i, -1,  1, S_LeftDown);
        TTT_EXE_CAPTURE(i,  0,  1, S_Down);
        TTT_EXE_CAPTURE(i,  1,  1, S_RightDown);

        board_[i].cap = cap;
    }
#undef TTT_EXE_CAPTURE
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

void Board::exeCapture_(Square m)
{
    const int cap = board_[m].cap;

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
        }                                            \
    }

    TTT_EXE_CAPTURE(m, -1, -1, S_LeftUp);
    TTT_EXE_CAPTURE(m,  0, -1, S_Up);
    TTT_EXE_CAPTURE(m,  1, -1, S_RightUp);
    TTT_EXE_CAPTURE(m, -1,  0, S_Left);
    TTT_EXE_CAPTURE(m,  1,  0, S_Right);
    TTT_EXE_CAPTURE(m, -1,  1, S_LeftDown);
    TTT_EXE_CAPTURE(m,  0,  1, S_Down);
    TTT_EXE_CAPTURE(m,  1,  1, S_RightDown);

#undef TTT_EXE_CAPTURE
}

void Board::makeMove(Move m)
{
    TTT_ASSERT(canMoveTo(stm_, m),
               "invalid move in Board::makeMove (" << m << " / " << toString(m) << ")");

#ifdef TTT_CAPTURE_WAIT
    // decrease the capture-block value
    for (auto &i : board_)
        i.v = (i.v & pieceMask) | (((i.v & captureLockedMask) >> 1) & captureLockedMask);
#endif

#ifdef TTT_CAPTURE
    // XXX Todo: only need m square here
    if (num_captures_ < 0)
        getCaptures_();
#endif

    setPieceAt(m, stm_);
    pieces_++;

#ifdef TTT_CAPTURE
    //exeCapture_(m);
    getCaptures_();
    // invalidate previous captures
    num_captures_ = -1;

/*    exeCapture(m, 1, 0);
    exeCapture(m, 1, 1);
    exeCapture(m, 0, 1);
    exeCapture(m,-1, 1);
    exeCapture(m,-1, 0);
    exeCapture(m,-1,-1);
    exeCapture(m, 0,-1);
    exeCapture(m, 1,-1);*/
#endif
}


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




bool Board::isDraw() const
{
    return (pieces() >= size() * size());
}

std::string Board::toString(Move m) const
{
    if (m == InvalidMove)
        return "invalid";
    else
    {
        std::string s;
        s += (char)((m%size_) + 'a');
        s += (char)((m/size_) + '1');
        return s;
    }
}

void Board::printBoard(bool eval, std::ostream& out) const
{
    const bool bigboard_ = false;
    const bool bitboard = true;//false;

    // -- header --

    out << "  ";
    for (uint x=0; x<size_; ++x)
        out << std::setw(2+bigboard_) << (char)(x + 'a');
    out << " ";

    if (bitboard)
    {
        out << "    |";
        for (uint x=0; x<size_; ++x)
            out << "    " << (char)(x + 'a');
    }

    if (eval)
    {
        out << "    | ";
        for (uint x=0; x<size_; ++x)
            out << std::setw(6) << (char)(x + 'a');
        out << "   ply: " << ply_ << " (" << pieceChar(stm_) << ")";
    }
    out << std::endl;

    for (uint y=0; y<size_; ++y)
    {
        for (int y1=0; y1<=bigboard_; ++y1)
        {
            if (!bigboard_ ^ y1)
                out << std::setw(2) << (y+1) << " ";
            else
                out << "   ";

            // ascii board
            for (uint x=0; x<size_; ++x)
            {
                if (!bigboard_)
                    out << pieceChar(board_[y*size_+x].v) << " ";
                else
                {
                    switch (board_[y*size_+x].v & pieceMask)
                    {
                    default: if (y1==0) out << "   ";  else out << ".  "; break;
                    case X:  if (y1==0) out << "\\/ "; else out << "/\\ "; break;
                    case O:  if (y1==0) out << "/\\ "; else out << "\\/ "; break;
                    }
                }
            }

            // bit board
            if (bitboard)
            {
                out << "    |";
                for (uint x=0; x<size_; ++x)
                {
                    std::cout << " ";
                    //for (int b=3; b>=0; --b)
                    //    std::cout << (int)(board_[y*size_+x].v & (1<<b));
                    for (int b=32; b>=0; --b)
                        std::cout << (int)((board_[y*size_+x].cap & (1<<b)) != 0);
                }
            }

            // eval board
            if (eval && y1==0)
            {
                out << "    | ";
                for (uint x=0; x<size_; ++x)
                {
                    if (pieceAt(y*size_+x) == Empty && score_[y*size_+x] != InvalidScore)
                        out << std::setw(6) << score_[y*size_+x];
                    else
                        out << std::setw(6) << " ";//(char)(pieceChar(pieceAt(y*size_+x))+32);
                }
            }

            out << std::endl;
        }
    }
}

void Board::clearEvalMap()
{
    for (auto &i : score_)
        i = InvalidScore;
}

void Board::setEvalMap(Square s, int score)
{
    score_[s] = score;
}

void Board::copyEvalFrom(const Board& b)
{
    for (size_t i=0; i<sizesq_; ++i)
        score_[i] = b.score_[i];
}


} // namespace TTT

