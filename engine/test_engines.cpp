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
#include "boardhelper.h"
#include "search.h"

using namespace TTT;


class TestEngines
{
public:

    static const int MAX_MOVES = 512;

    struct Stats
    {
        int num_games,
            num_draws,
            num_wins[2],
            num_invalid_moves,
            num_max_moves_reached,
            num_moves_histogram[MAX_MOVES+1];
    };
    Stats stat;


    TestEngines()
        :   bh(b)
    {
        maxDepth[0] = maxDepth[1] = 4;
    }


    void play_game()
    {
        b.init();

#define TTT_STOP { playing = false; continue; }

        bool playing = true;
        while (playing)
        {
            size_t
                stm = b.stm() - 1
                //nstm = !stm
                    ;

            Move m = ai[stm].bestMove(b, maxDepth[stm]);
            if (m == InvalidMove)
            {
                stat.num_invalid_moves++;
                TTT_STOP
            }

            // -- make move --

            b.makeMove(m);

            if (b.numPly() > MAX_MOVES-1)
            {
                stat.num_max_moves_reached++;
                TTT_STOP
            }

            // -- check result --

            if (b.isDraw())
            {
                stat.num_draws++;
                TTT_STOP
            }
            else
            if (bh.isWin(b, b.stm()))
            {
                stat.num_wins[stm]++;
                TTT_STOP;
            }

            b.flipStm();
        }
#undef TTT_STOP

        stat.num_games++;
        stat.num_moves_histogram[b.numPly()]++;
    }

    void printStats(std::ostream& out = std::cout)
    {
        out << "games " << stat.num_games
            << " X=" << stat.num_wins[0] << " O=" << stat.num_wins[1]
            << " (" << stat.num_draws << ")"
            << std::endl;
    }


    void run()
    {
        bh.setSize(b);
        memset(&stat, 0, sizeof(stat));

        play_game();
    }

    // ----------- public member ------------

    Board b;
    BoardHelper bh;
    Search ai[2];

    // --- config ---

    int maxDepth[2];

    // --- stats ----

};





void test_engines()
{

    TestEngines test;

    test.run();

}
