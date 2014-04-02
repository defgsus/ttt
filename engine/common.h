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

#ifndef TTT_ENGINE_COMMON_H
#define TTT_ENGINE_COMMON_H

// --------- compile settings --------------

/** Multi-threading support (fixed to 8 in Search::bestMove())
    NOT USED RIGHT NOW */
//#define TTT_THREADED
/** Allow captures */
#define TTT_CAPTURE
/** Force one move pause for captured squares */
#define TTT_CAPTURE_WAIT
/** Use Alpha-Beta-Pruning */
#define TTT_ALPHA_BETA
/** Auto-limit level proportional to number of root nodes */
#define TTT_LIMIT_DEPTH
/** Use a greedyness value to cut-off nodes
    NOT USED RIGHT NOW */
//#define TTT_GREEDY
/** randomize moves a little */
//#define TTT_RANDOMNESS
/** Use a hash table (worse performance in threading mode
    NOT USED RIGHT NOW */
//#define TTT_TRANSPOSITION_TABLE
/** Keep the entire search tree (for display).
    NOT USED RIGHT NOW */
//#define TTT_KEEP_TREE

#ifndef NDEBUG
#   include <cstdlib>
#   define TTT_DEBUG(stream_arg__) { std::cerr << stream_arg__ << std::endl; }
#   define TTT_ASSERT(expr__, stream_arg__) { \
        if (!(expr__)) { std::cerr << stream_arg__ << "\n" << __FILE__ << " : " << __LINE__ << std::endl; exit(-1); } }
#else
#   define TTT_DEBUG(unused__) { }
#   define TTT_ASSERT(expr__, unused__) { }
#endif

#ifndef TTT_NO_PRINT
#   define TTT_PRINT(stream_arg__) { std::cerr << stream_arg__ << std::endl; }
#else
#   define TTT_PRINT(unused__) { }
#endif

#include <vector>
#include <string>
#include <iostream>
#include <cinttypes>


namespace TTT {


typedef unsigned int        uint;

typedef uint32_t            Piece;
//typedef unsigned char       Piece;
typedef uint                Square;
typedef Square              Move;
typedef std::vector<Move>   Moves;
typedef Piece               Stm;


/** That's what goes onto a board square.
 *
 * always assumed: first bit is you, second bit is other
 *
 * Current Square/Piece layout:
 * bit 0   : X
 * bit 1   : O
 * bit 2-3 : capture wait counter
 *
 */
enum PieceType
{
    Empty, X = 1, O = 2
};

const Move InvalidMove = -1;
const int MaxScore = 7000;
const int WinScore = MaxScore / 2;
const int InvalidScore = MaxScore*10;


#ifdef TTT_TRANSPOSITION_TABLE
struct Hash
{
    typedef uint64_t Type;
    Type a, b;

    static void getHash(Hash& h, const Piece * board, uint s, Stm stm)
    {
        h.a = h.b = 0;
        Type * a = &h.a;
        for (uint i=0; i<s; ++i, ++board)
        {
            if (i == 32) a = &h.b;
            *a |= *board & pieceMask;
            *a <<= 2;
        }
        h.b |= ((Type)stm) << 62;
    }

    bool operator == (const Hash& r) const
    {
        return r.a == a && r.b == b;
    }

    bool operator < (const Hash& r) const
    {
        return (b == r.b)? (a < r.a) : (b < r.b);
    }

};
#endif

template <typename S>
S abs(S n)
{
    return n < 0? -n : n;
}


/** good ol' implicit ulam spiral for move ordering
 *  [starting at zero] */
template <typename I>
static I ulam_spiral(I x, I y)
{
    I d;
    if ((d= x)>abs(y)) return (d * 4 - 3) * d + y;
    if ((d=-x)>abs(y)) return (d * 4 + 1) * d - y;
    {
        d = abs(y);
        if (y > 0) return (d * 4 - 1) * d - x;
              else return (d * 4 + 3) * d + x;
    }
}



} // namespace TTT

#endif // TTT_ENGINE_COMMON_H
