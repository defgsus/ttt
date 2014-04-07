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

/*
#define BOARD_INC_UP         ((int)(-size_))
#define BOARD_INC_LEFT_UP    ((int)(-size_ - 1))
#define BOARD_INC_RIGHT_UP   ((int)(-size_ + 1))
#define BOARD_INC_LEFT       ((int)-1)
#define BOARD_INC_RIGHT      ((int)1)
#define BOARD_INC_LEFT_DOWN  ((int)(size_-1))
#define BOARD_INC_DOWN       ((int)(size_))
#define BOARD_INC_RIGHT_DOWN ((int)(size_+1))
*/

namespace TTT {


// -------------------------- board ---------------------------

Board::Board(uint size, uint cons)
    :   size_   (std::min(maxBoardSize, size)),
        sizesq_ (size_ * size_),
        cons_   (cons),
        board_  (sizesq_),
        cap_    (sizesq_),
        score_  (sizesq_)
{
    TTT_DEBUG("Board::Board(" << size << ", " << cons << ")");
    init();
}

void Board::init()
{
    for (auto &i : board_)
        i = Empty;
    for (auto &i : cap_)
        i = 0;
    clearEvalMap();

    stm_ = X;
    nstm_ = O;
    pieces_ = 0;
    ply_ = 0;

#ifdef TTT_CAPTURE
    num_captures_ = -1;
    num_last_captured_ = 0;
    num_all_captured_[X] = num_all_captured_[O] = 0;
#endif
}

void Board::setSize(uint size, uint cons)
{
    size_ = std::max(2u, std::min(maxBoardSize, size ));
    sizesq_ = size_ * size_;
    cons_ = std::min(cons, size_);
    board_.resize(sizesq_);
    cap_.resize(sizesq_);
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
        board_[i] = p;
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



void Board::makeMove(Move m)
{
    TTT_ASSERT(canMoveTo(stm_, m),
               "invalid move in Board::makeMove (" << m << " / " << toString(m) << ")");

#ifdef TTT_CAPTURE_WAIT
    // decrease the capture-block value
    for (auto &i : board_)
       i = (i & pieceMask) | (((i & captureLockedMask) >> 1) & captureLockedMask);
#endif

#ifdef TTT_CAPTURE
    // XXX Todo: only need m square here
    if (num_captures_ < 0)
        getCapture_(m);
#endif

    setPieceAt(m, stm_);
    pieces_++;

#ifdef TTT_CAPTURE
    num_last_captured_ = exeCapture_(m);
    num_all_captured_[stm_] += num_last_captured_;

    // invalidate previous captures
    num_captures_ = -1;
#endif
}






bool Board::isDraw() const
{
    int u = 0;
    for (size_t i = 0; i < board_.size(); ++i)
    {
        u += (pieceAt(i) == Empty) && (!blockedAt(i));
    }
    return u == 0;
    //return (pieces() >= size() * size());
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
    const bool bitboard = false;

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
                    out << pieceChar(board_[y*size_+x]) << " ";
                else
                {
                    switch (board_[y*size_+x] & pieceMask)
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
                    //    std::cout << (int)(board_[y*size_+x] & (1<<b));
                    for (int b=31; b>=0; --b)
                        std::cout << (int)((cap_[y*size_+x] & (1<<b)) != 0)
                                << ((b&3)==0? " " : "");
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

