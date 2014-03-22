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
std::vector<Square> Board::moveOrder_;

Board::Board(uint size, uint cons)
    :   size_   (size),
        sizesq_ (size*size),
        cons_   (cons),
        board_  (sizesq_),
        score_  (sizesq_)
{
    createRowValues();
    createMoveOrder();
    init();
}

void Board::setSize(uint size, uint cons)
{
    size_ = size;
    sizesq_ = size*size;
    cons_ = cons;
    board_.resize(size*size);
    score_.resize(size*size);
    clearEvalMap();
    rowVal_.clear();
    createRowValues();
    moveOrder_.clear();
    createMoveOrder();
    init();
}

void Board::init()
{
    for (auto &i : board_)
        i = Empty;
    for (auto &i : score_)
        i = Empty;
    clearEvalMap();

    stm_ = X;
    nstm_ = O;
    pieces_ = 0;
    ply_ = 0;
}

Stm Board::flipStm()
{
    ply_++;
    std::swap(stm_, nstm_);
    return stm_;
}

void Board::createMoveOrder()
{
    if (!moveOrder_.empty()) return;

    moveOrder_.resize(sizesq_);
    for (auto &i : moveOrder_)
        i = 0;

    // use an ulam spiral to create indexes
    // that start in the middle and expand towards edges
    const int sh = (size_ >> 1) - !(size_&1);
    for (int y = 0; y<(int)size_; ++y)
    for (int x = 0; x<(int)size_; ++x)
    {
        size_t u = ulam_spiral(x-sh,y-sh);

        if (u<=moveOrder_.size())
            moveOrder_[u] = (y * size_ + x);
    }

    //for (auto i : moveOrder_)
    //    std::cout << " " << i;
}

void Board::createRowValues()
{
    if (!rowVal_.empty()) return;

    // creat all permutations of empty/self/other for a row of length cons_
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
            kk = k>>(i*2);
            if ((kk&3)==Empty) row.push_back(Empty); else
            if ((kk&3)==X)     row.push_back(X); else
            if ((kk&3)>=O)     row.push_back(O);
        }

        // get utility value
        int u = 0, full = 0;

        // start by scoring
        //   square values as:
        for (i = 0; i<cons_; ++i)
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
            for (i=0; i<cons_-1; ++i)
                if (row[i] == 1 && row[i+1] == 1)
                    u *= 2;

            // generally less points when opponent breaks row
            for (i = 0; i<cons_; ++i)
                if (row[i] == 2) { u /= 3; }
#if 1
            // no points for full-row-emptyness
            if (full == 0) u = 0;
#endif

        }

        // store utility value
        rowVal_.push_back( std::max(0, u));

#if 1
        // debug print
        if (k%4 != 3)
        {
            std::cout << std::setw(4) << k << " ";
            for (i=0; i<cons_; ++i) std::cout << pieceChar[row[i]];
            if (u>0) std::cout << " " << u;
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
    if (board_[x] != Empty)
        return InvalidMove;

    return x;
}

void Board::makeMove(Move m)
{
    assert(m < sizesq_ && board_[m] == Empty &&
           "invalid move in Board::makeMove");

    board_[m] = stm_;
    pieces_++;

    exeCapture(m, 1, 0);
    exeCapture(m, 1, 1);
    exeCapture(m, 0, 1);
    exeCapture(m,-1, 1);
    exeCapture(m,-1, 0);
    exeCapture(m,-1,-1);
    exeCapture(m, 0,-1);
    exeCapture(m, 1,-1);
/*
#ifdef TTT_CAPTURE
    // check for captures
    #define TTT_EXECAPTURE(x_,y_) \
        if (canCapture(m, x_, y_)) \
            { board_[m+x_+y_*size_] = Empty; board_[m+(x_+1)+(y_+1)*size_] = Empty; pieces_--; }
                //else

        TTT_EXECAPTURE( 1, 0)
        TTT_EXECAPTURE( 1, 1)
        TTT_EXECAPTURE( 0, 1)
        TTT_EXECAPTURE(-1, 1)
        TTT_EXECAPTURE(-1, 0)
        TTT_EXECAPTURE(-1,-1)
        TTT_EXECAPTURE( 0,-1)
        TTT_EXECAPTURE( 1,-1)
        //{ }

    #undef TTT_EXECAPTURE

    //if (board_[m] == Empty) pieces_--;
#endif
*/
}

bool Board::exeCapture(Square m, int xi, int yi)
{
    if (board_[m] != stm_) return false;

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

        if (board_[pos] == Empty)
            return false;
        else
        if (board_[pos] == nstm_)
            ++op;
        else
        if (board_[pos] == stm_)
        {
            if (op>0)
            {
                // remove pieces
                for (int j=0; j<op; ++j)
                {
                    pos -= inc;
                    board_[pos] = Empty;
                }
                pieces_ -= op;
                return true;
            }
            return false;
        }
    }
    return false;
/*
    const int x = m%size_, y = m/size_;

    // board limits
    if ((xi<0 && x<2)
     || (yi<0 && y<2)
     || (xi>0 && x>(int)size_-3)
     || (yi>0 && y>(int)size_-3))
        return false;

    // check if opponent is captured
    bool r= (board_[(y+yi)*size_ + x+xi] == nstm_
            && board_[(y+yi*2)*size_ + x+xi*2] == stm_);
    //if (r) std::cout << "captured for " << toString(m) << "\n";
    return r;
*/
}

void Board::getMoves(Moves &m) const
{
    m.clear();
    for (size_t i=0; i<moveOrder_.size(); ++i)
    {
        const Square k = moveOrder_[i];

        if (board_[k] == Empty)
            m.push_back(k);
    }
}


bool Board::isWin(Stm p) const
{
    return (eval(p) >= WinScore);
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

    out << "  ";
    for (uint x=0; x<size_; ++x)
        out << std::setw(2+bigboard_) << (char)(x + 'a');
    if (eval)
    {
        out << "    | ";
        for (uint x=0; x<size_; ++x)
            out << std::setw(6) << (char)(x + 'a');
        out << "   ply: " << ply_ << " (" << pieceChar[stm_] << ")";
        out << std::endl;
    }

    for (uint y=0; y<size_; ++y)
    {
        for (int y1=0; y1<=bigboard_; ++y1)
        {
            if (!bigboard_ ^ y1)
                out << std::setw(2) << (y+1) << " ";
            else
                out << "   ";

            for (uint x=0; x<size_; ++x)
            {
                if (!bigboard_)
                    out << pieceChar[board_[y*size_+x]] << " ";
                else
                {
                    switch (board_[y*size_+x])
                    {
                    default: if (y1==0) out << "   ";  else out << ".  "; break;
                    case X:  if (y1==0) out << "\\/ "; else out << "/\\ "; break;
                    case O:  if (y1==0) out << "/\\ "; else out << "\\/ "; break;
                    }
                }
            }

            if (eval && y1==0)
            {
                out << "   | ";
                for (uint x=0; x<size_; ++x)
                {
                    if (board_[y*size_+x] == Empty && score_[y*size_+x] != InvalidScore)
                        out << std::setw(6) << score_[y*size_+x];
                    else
                        out << std::setw(6) << " ";//(char)(pieceChar[board_[y*size_+x]]+32);
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


int Board::eval()
{
    return eval(stm_) - eval(nstm_);
    /*
    int x = eval(X), o = eval(O);
    if (x>=MaxScore) x *= 2;
    if (o>=MaxScore) o *= 2;
    int v = x-o;

    if (x>=MaxScore)
        v = x;
    else if (o>=MaxScore)
        v = -o;

    return (stm_ == X)? v : -v;
    */
}




int Board::eval(Stm side) const
{
    if (side == Empty) return 0;

    Stm nside = side ^ 3;

    int u = 0;
    uint x,y;

    // --- find consecutives ---

// start at x,y, go along direction xi,yi, bit-fiddle row value
// which is an index into rowVal_[]
#define TTT_CHECK(xi, yi)                   \
    {                                       \
        uint cx = x, cy = y, cnt=0;         \
        for (uint i=0; i<cons_; ++i)        \
        {                                   \
            cnt <<= 2;                      \
            Piece p = board_[cy*size_+cx];  \
            cnt += ((p&side)!=0) |          \
                   (((p&nside)!=0)<<1);     \
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




