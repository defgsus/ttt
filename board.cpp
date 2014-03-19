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

#include <cassert>
#include <cmath>
#include <iomanip>

std::vector<int> Board::rowVal_;

Board::Board(unsigned int size, unsigned int cons)
    :   size_   (size),
        cons_   (cons),
        board_  (size*size)
{
    createRowValues();
    init();
}

void Board::init()
{
    for (auto &i : board_)
        i = Empty;

    stm_ = X;
    pieces_ = 0;
}

void Board::createRowValues()
{
    if (!rowVal_.empty()) return;

    // creat all permutations of empty/X/O for a row of length cons_
    // and precalculate the utility value
    unsigned int i,j,kk,k = 0, num = pow(4, cons_);
    for (j=0; j<num; ++j, ++k)
    {
        std::vector<int> row;
        for (i=0; i<cons_; ++i)
        {
            kk = k>>(i*2);
            if ((kk&3)==0) row.push_back(0); else
            if ((kk&3)==1) row.push_back(1); else
            if ((kk&3)>=2) row.push_back(2);
        }

        // get utility value
        unsigned int u = 0;

        // count empty=1, piece=2
        for (i = 0; i<cons_; ++i)
            u += std::min(1, row[i]) + 1;

        // full row?
        if (u>=cons_*2)
        {
            u = MaxScore; // win
        }
        else
        {
            // additional points for consecutiveness
            for (i=0; i<cons_-1; ++i)
                if (row[i] == 1 && row[i+1] == 1)
                    u++;
        }

        // no points when opponent breaks row
        for (i = 0; i<cons_; ++i)
            if (row[i] == 2) { u = 0; break; }

#if 0
        // no points for emptyness
        cnt = 0;
        for (i = 0; i<cons_; ++i)
            cnt += row[i] == 0;
        if (cnt == cons_) u = 0;
#endif
        // store utility value
        rowVal_.push_back(u);

#if 0
        // debug print
        if (k%4 != 3)
        {
            std::cout << std::setw(4) << k << " ";
            for (i=0; i<cons_; ++i) std::cout << pieceChar[row[i]];
            if (u) std::cout << " " << u;
            std::cout << std::endl;
        }
#endif
    }
}

void Board::init(const std::string& str)
{
    init();
    for (size_t i=0; i<str.length() && i<board_.size(); ++i)
    {
        Piece p = Empty;
        switch (str[i])
        {
            case 'x': case 'X': p = X; pieces_++; break;
            case 'o': case 'O': p = O; pieces_++; break;
        }
        board_[i] = p;
    }
}

Piece Board::flipStm()
{
    if (stm_ == X)
        stm_ = O;
    else
        stm_ = X;
    return stm_;
}

Move Board::parseMove(const std::string& str) const
{
    if (str.size() < 2)
        return InvalidMove;

    unsigned int x, y;

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
    if (board_[x] != Empty)
        return InvalidMove;

    return x;
}

void Board::makeMove(Move m)
{
    assert(m < size_*size_ && board_[m] == Empty &&
           "invalid move in Board::makeMove");

    board_[m] = stm_;
    pieces_++;
}

void Board::getMoves(Moves &m) const
{
    m.clear();
    for (size_t i = 0; i<board_.size(); ++i)
        if (board_[i] == Empty)
            m.push_back(i);
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

void Board::printBoard(std::ostream& out) const
{
    out << "   ";
    for (unsigned int x=0; x<size_; ++x)
        out << (char)(x + 'a') << " ";
    out << std::endl;

    for (unsigned int y=0; y<size_; ++y)
    {
        out << std::setw(2) << (y+1) << " ";

        for (unsigned int x=0; x<size_; ++x)
        {
            out << pieceChar[board_[y*size_+x]] << " ";
        }

        out << std::endl;
    }
}

int Board::eval()
{
    int x = eval(X), o = eval(O);
    int v = x-o;
    /*
    if (x>=MaxScore)
        v = x;
    else if (o>=MaxScore)
        v = o;
    */
    return (stm_ == X)? v : -v;
}

#if (0)
int Board::eval(PieceType p) const
{
    if (p == Empty) return 0;

    int u = 0, c;
    unsigned int x,y;

    // --- find consecutives ---

#define TTT_ADD                  \
    if (c>=(int)cons_) return MaxScore;   \
    if (c>1) u += c-1;
    //u += c;

#define TTT_CHECK(expr_)         \
    if ((expr_))                 \
        ++c;                     \
    else                         \
    {                            \
        TTT_ADD                  \
        c = 0;                   \
    }

    // horizontal
    for (y=0; y<size_; ++y)
    {
        c = 0;
        for (x=0; x<size_; ++x)
        {
            TTT_CHECK(board_[y*size_+x] == p);
        }
        TTT_ADD
    }

    // vertically
    for (x=0; x<size_; ++x)
    {
        c = 0;
        for (y=0; y<size_; ++y)
        {
            TTT_CHECK(board_[y*size_+x] == p);
        }
        TTT_ADD
    }

    // diagonal
    c = 0;
    for (x=0; x<size_; ++x)
    {
        TTT_CHECK(board_[x*size_+x] == p);
    }
    TTT_ADD

    c = 0;
    for (x=0; x<size_; ++x)
    {
        TTT_CHECK(board_[x*size_+size_-1-x] == p);
    }
    TTT_ADD

    #undef TTT_ADD
    #undef TTT_CHECK

    return u;
}
#endif

#if (0)
int Board::eval(PieceType p) const
{
    if (p == Empty) return 0;

    int u = 0;
    unsigned int x,y;

    // --- find consecutives ---

#define TTT_CHECK(xi, yi)                   \
    {                                       \
        unsigned int cx = x, cy = y,        \
                     cnt = 0;               \
        for (unsigned int i=0; i<cons_; ++i)\
        {                                   \
            Piece b = board_[cy*size_+cx];  \
            if (b == p)                     \
                cnt += 2;                   \
            else if (b == Empty)            \
            {   if (cnt) cnt += 1; }        \
            else                            \
            {                               \
                cnt = 0;                    \
            }                               \
            cx += xi; cy += yi;             \
        }                                   \
        if (cnt >= cons_*2) return 1000;    \
        u += cnt;                           \
    }

    // horizontal
    for (y=0; y<size_; ++y)
    {
        for (x=0; x<=size_-cons_; ++x)
            TTT_CHECK(1,0);
        for (x=size_-1; x>=cons_-1; --x)
            TTT_CHECK(-1,0);
    }

    // vertically
    for (x=0; x<size_; ++x)
    {
        for (y=0; y<=size_-cons_; ++y)
            TTT_CHECK(0,1);
        for (y=size_-1; y>=cons_-1; --y)
            TTT_CHECK(0,-1);
    }

    // diagonally right
    for (y=0; y<=size_-cons_; ++y)
    for (x=0; x<=size_-cons_; ++x)
        TTT_CHECK(1,1);
    for (y=size_-1; y>=cons_-1; --y)
    for (x=size_-1; x>=cons_-1; --x)
        TTT_CHECK(-1,-1);

    // diagonally left
    for (y=0; y<=size_-cons_; ++y)
    for (x=size_-1; x>=cons_-1; --x)
        TTT_CHECK(-1,1);
    for (y=size_-1; y>=cons_-1; --y)
    for (x=0; x<=size_-cons_; ++x)
        TTT_CHECK(1,-1);

    #undef TTT_CHECK

    return u;
}
#endif

int Board::eval(PieceType p) const
{
    if (p == Empty) return 0;

    int u = 0;
    unsigned int x,y;

    // --- find consecutives ---

// start at x,y, go along direction xi,yi, count row value
#define TTT_CHECK(xi, yi)                   \
    {                                       \
        unsigned int cx = x, cy = y, cnt=0; \
        for (unsigned int i=0; i<cons_; ++i)\
        {                                   \
            cnt <<= 2;                      \
            Piece b = board_[cy*size_+cx];  \
            if (b == p)                     \
                ++cnt;                      \
            else if (b != Empty)            \
                cnt += 2;                   \
            cx += xi; cy += yi;             \
        }                                   \
    /*std::cout << ":" << cnt << "\n";*/ \
        u += rowVal_[cnt];                  \
        if (u >= MaxScore) return MaxScore; \
    }

    // horizontal
    for (y=0; y<size_; ++y)
    {
        for (x=0; x<=size_-cons_; ++x)
            TTT_CHECK(1,0);
    }

    // vertically
    for (x=0; x<size_; ++x)
    {
        for (y=0; y<=size_-cons_; ++y)
            TTT_CHECK(0,1);
    }

    // diagonally right
    for (y=0; y<=size_-cons_; ++y)
    for (x=0; x<=size_-cons_; ++x)
        TTT_CHECK(1,1);

    // diagonally left
    for (y=0; y<=size_-cons_; ++y)
    for (x=size_-1; x>=cons_-1; --x)
        TTT_CHECK(-1,1);

    #undef TTT_CHECK

    return u;
}




