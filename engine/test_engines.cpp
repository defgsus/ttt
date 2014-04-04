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

#include <iomanip>

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
            num_moves,
            num_invalid_moves,
            num_max_moves_reached,
            num_moves_histogram[MAX_MOVES+1],

            num_wins_per_square[maxBoardSize*maxBoardSize*2];
    };
    Stats stat;


    TestEngines()
        :   b(5,4),
            bh(b)
    {
        maxDepth[0] = maxDepth[1] = 4;
        captureWeight[0] = captureWeight[1] = 0;

        do_50_start = false;
        do_fixed_first_move = true;
    }


    void play_game()
    {
        ai[0].captureWeight = captureWeight[0];
        ai[1].captureWeight = captureWeight[1];

        b.init();

        // 50/50 start turn
        if (do_50_start)
        {
            if (stat.num_games & 1)
                b.flipStm();
        }

        int start_square = -1;

        // start with fixed move
        if (do_fixed_first_move)
        {
            b.makeMove(start_square = stat.num_games % (b.size()*b.size()));
            stat.num_moves++;
            b.flipStm();
        }

        // ---- game loop ----

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

            // store first move
            if (start_square<0)
                start_square = m;

            // -- make move --

            b.makeMove(m);
            stat.num_moves++;

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
                if (start_square>=0)
                    stat.num_wins_per_square[start_square*2+stm]++;
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
        float winx = 0, avmove = 0;
        if (stat.num_wins[0] + stat.num_wins[1] > 0)
            winx = (float)stat.num_wins[0] / (stat.num_wins[0] + stat.num_wins[1]) * 100;
        if (stat.num_games>0)
            avmove = (float)stat.num_moves / stat.num_games;

        // settings
        out << "board " << b.size() << " " << b.consecutives()
            << "  depth " << maxDepth[0] << ":" << maxDepth[1]
            << "  capture weight " << captureWeight[0] << ":" << captureWeight[1]
            << "\nstart-side " << (do_50_start? "X/O" : "X")
            << "  fixed-first-move " << (do_fixed_first_move? "yes" : "no")
            << "\n";
        // stats
        out << "games " << stat.num_games
            << "  wins " << stat.num_wins[0] << ":" << stat.num_wins[1]
            << " (" << winx << "%)"
            << "  draws " << stat.num_draws
            << "\n"
            << "moves " << stat.num_moves
            << "  average " << avmove
            << "  invalid " << (stat.num_invalid_moves) << "\n"
            << "wins per start square:\n";
        for (size_t j=0; j<b.size(); ++j)
        {
            for (size_t i=0; i<b.size(); ++i)
            {
                const int winx = stat.num_wins_per_square[(j*b.size()+i)*2],
                          wino = stat.num_wins_per_square[(j*b.size()+i)*2+1];
                if (winx)
                    out << std::right << std::setw(5) << winx << ":";
                else
                    out << std::right << std::setw(5) << " " << ":";
                if (wino)
                    out << std::left << std::setw(4) << wino << std::right;
                else
                    out << std::left << std::setw(4) << " " << std::right;
            }
            out << "\n";
        }

        out << std::endl;
    }

    void run_test()
    {
        bh.setSize(b);
        memset(&stat, 0, sizeof(stat));

        //const int num = 3000;
        const int num = b.size() * b.size();
        for (int i=0; i<num; ++i)
        {
            //if (i!=0 && i%10==0)
            //    printStats();

            play_game();

        }

        printStats();
        printStats(std::cerr);
    }

    void run()
    {
        for (int i=0; i<=10; ++i)
        {
            maxDepth[0] =
            maxDepth[1] = 6;
            captureWeight[0] = i * 10;
            run_test();
        }
    }

    // ----------- public member ------------

    Board b;
    BoardHelper bh;
    Search ai[2];

    // --- config ---

    int maxDepth[2], captureWeight[2];
    bool
        do_50_start,
        do_fixed_first_move;
    ;
    // --- stats ----

};





void test_engines()
{

    TestEngines test;

    test.run();

}



