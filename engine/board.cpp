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

namespace TTT {

void BoardHelper::createMoveOrder()
{
    moveOrder.resize(size * size);

    for (auto &i : moveOrder)
        i = 0;

    // use an ulam spiral to create indexes
    // that start in the middle and expand towards edges
    const int sh = (size >> 1) - !(size&1);
    for (int y = 0; y<(int)size; ++y)
    for (int x = 0; x<(int)size; ++x)
    {
        size_t u = ulam_spiral(x-sh,y-sh);

        if (u<=moveOrder.size())
            moveOrder[u] = (y * size + x);
    }

    //for (auto i : moveOrder_)
    //    std::cout << " " << i;
}

int BoardHelper::getRowValue(int * row, const int X, const int O) const
{
    // get utility value
    int u = 0, full = 0;

    // start by scoring
    //   square values as:
    for (uint i = 0; i<cons; ++i)
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
    if (full>=(int)cons)
    {
        u = MaxScore; // win
    }
    else
    {
        // additional points for consecutiveness
        for (uint i=0; i<cons-1; ++i)
            if (row[i] == X && row[i+1] == X)
                u *= 2;

        // generally less points when opponent breaks row
        for (uint i = 0; i<cons; ++i)
            if (row[i] == O) { u /= 3; break; }
#if 1
        // no points for full-row-emptyness
        if (full == 0) u = 0;
#endif

    }

    return std::max(0,u);
}


void BoardHelper::createRowValues()
{
    rowValues.clear();

    // creat all permutations of empty/self/other for a row of length cons
    // and precalculate the utility value
    // Note.
    // The value '3' in the 2 piece bits is not used.
    // The map still contains a value there for efficient access
    // (entry = rowvalue<<2)
    uint i,j,kk,k = 0, num = pow(4, cons);
    for (j=0; j<num; ++j, ++k)
    {
        std::vector<int> row;
        for (i=0; i<cons; ++i)
        {
            kk = (k>>(i*2)) & pieceMask;
            if ((kk)==Empty) row.push_back(Empty); else
            if ((kk)==X)     row.push_back(X); else
            if ((kk)>=O)     row.push_back(O);
        }

        int u = getRowValue(&row[0], X, O)
              - getRowValue(&row[0], O, X);

        // store utility value
        rowValues.push_back( u );

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


void BoardHelper::createScanOrder()
{
    scanOrder.clear();

#define TTT_SCAN(xi, yi)                       \
    {                                          \
        uint cx = x, cy = y;                   \
        for (uint i=0; i<cons; ++i)            \
        {                                      \
            scanOrder.push_back(cy*size+cx);   \
            cx += xi; cy += yi;                \
        }                                      \
    }

    uint x,y;

    // horizontal
    for (y=0; y<size; ++y)
        for (x=0; x<=size-cons; ++x)
            TTT_SCAN(1,0);

    // vertically
    for (x=0; x<size; ++x)
        for (y=0; y<=size-cons; ++y)
            TTT_SCAN(0,1);

    // diagonally right
    for (y=0; y<=size-cons; ++y)
        for (x=0; x<=size-cons; ++x)
            TTT_SCAN(1,1);

    // diagonally left
    for (y=0; y<=size-cons; ++y)
        for (x=size-1; x>=cons-1; --x)
            TTT_SCAN(-1,1);

    #undef TTT_SCAN

}


















// -------------------------- board ---------------------------

Board::Board(uint size, uint cons)
    :   helper_ (new BoardHelper(size, cons)),
        ownHelper_(true),
        size_   (size),
        sizesq_ (size*size),
        cons_   (cons),
        board_  (sizesq_),
        score_  (sizesq_)
{
    init();
}

Board::~Board()
{
    if (ownHelper_)
        delete helper_;
}

Board::Board(const Board& r)
{
    *this = r;
}

Board& Board::operator = (const Board& r)
{
    helper_ = r.helper_;
    ownHelper_ = false;

    size_ = r.size_;
    sizesq_ = r.sizesq_;
    cons_ = r.cons_;

    stm_ = r.stm_;
    nstm_ = r.nstm_;
    pieces_ = r.pieces_;
    ply_ = r.ply_;

    board_ = r.board_;
    score_ = r.score_;

    return *this;
}


void Board::setSize(uint size, uint cons)
{
    if (ownHelper_)
        delete helper_;

    helper_ = new BoardHelper(size, cons);
    ownHelper_ = true;

    size_ = size;
    sizesq_ = size*size;
    cons_ = cons;
    board_.resize(size*size);
    score_.resize(size*size);
    clearEvalMap();
    init();
}

void Board::init()
{
    for (auto &i : board_)
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
        && !isCaptured(m)
#endif
    ;
}


void Board::makeMove(Move m)
{
    assert(canMoveTo(stm_, m)
           && "invalid move in Board::makeMove");

    #ifdef TTT_CAPTURE_WAIT
    for (auto &i : board_)
        i = (i & pieceMask) | (((i & captureMask) >> 1) & captureMask);
    #endif

    setPieceAt(m, stm_);
    pieces_++;

#ifdef TTT_CAPTURE
    exeCapture(m, 1, 0);
    exeCapture(m, 1, 1);
    exeCapture(m, 0, 1);
    exeCapture(m,-1, 1);
    exeCapture(m,-1, 0);
    exeCapture(m,-1,-1);
    exeCapture(m, 0,-1);
    exeCapture(m, 1,-1);
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

void Board::getMoves(Moves &m) const
{
    m.clear();
    for (size_t i=0; i<helper_->moveOrder.size(); ++i)
    {
        const Square k = helper_->moveOrder[i];

        if (canMoveTo(stm_, k))
            m.push_back(k);
    }
}


bool Board::isWin(Stm p) const
{
    int e = evalX();
    return ((p == X && e >= WinScore)
            || (p == O && -e >= WinScore));
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
                    switch (board_[y*size_+x])
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
                    for (int b=3; b>=0; --b)
                        std::cout << (int)(board_[y*size_+x] & (1<<b));
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


int Board::eval()
{
    return (stm_==X)? evalX() : -evalX();// - eval(nstm_);
}




int Board::evalX() const
{
    int u = 0;

    // --- find consecutives ---

    for (uint i=0; i<helper_->scanOrder.size(); )
    {
        // bit-fiddle row value
        // which is an index into rowVal_[]
        uint cnt = 0;
        for (uint j=0; j<cons_; ++j, ++i)
        {
            cnt <<= 2;
            cnt += pieceAt(helper_->scanOrder[i]);
        }
        // add evaluation value
        u += helper_->rowValues[cnt];
    }

    return u;
}


} // namespace TTT

