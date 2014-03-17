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

Board::Board(unsigned int size)
    :   size_   (size),
        board_  (size*size)
{
    init();
}

void Board::init()
{
    for (auto &i : board_)
        i = Empty;
    stm_ = X;
}

void Board::init(const std::string& str)
{
    init();
    for (size_t i=0; i<str.length() && i<board_.size(); ++i)
    {
        Piece p = Empty;
        switch (str[i])
        {
        case 'x': case 'X': p = X; break;
        case 'o': case 'O': p = O; break;
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
    board_[m] = stm_;
}

void Board::getMoves(Moves &m) const
{
    m.clear();
    for (size_t i = 0; i<board_.size(); ++i)
        if (board_[i] == Empty)
            m.push_back(i);
}

void Board::printBoard(std::ostream& out) const
{
    out << "   ";
    for (unsigned int x=0; x<size_; ++x)
        out << (char)(x + 'A');
    out << std::endl;

    for (unsigned int y=0; y<size_; ++y)
    {
        out << std::setw(2) << (y+1) << " ";

        for (unsigned int x=0; x<size_; ++x)
        {
            switch (board_[y*size_+x])
            {
                default: out << "."; break;
                case X: out << "X"; break;
                case O: out << "O"; break;
            }
        }

        out << std::endl;
    }
}

int Board::eval(PieceType p) const
{
    if (p == Empty) return 0.f;

    int u = 0, c;
    unsigned int x,y;

    // --- find consecutives ---

#define TTT_ADD                  \
    if (c>=(int)size_) return 1000;   \
    if (c>1) u += c-1;

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

