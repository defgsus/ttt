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
        maxDepth[0] = maxDepth[1] = 2;

        do_50_start = false;
        do_fixed_first_move = true;
    }


    void play_game()
    {
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
            << "  depth " << maxDepth[0] << ":" << maxDepth[1] << "\n"
            << "start-side " << (do_50_start? "X/O" : "X")
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
    }

    void run()
    {
        for (int i=2; i<=10; ++i)
        {
            maxDepth[0] = i;
            maxDepth[1] = 4;
            run_test();
        }
    }

    // ----------- public member ------------

    Board b;
    BoardHelper bh;
    Search ai[2];

    // --- config ---

    int maxDepth[2];
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



/*

depth 4:4  start side X  random first move yes
games 3000 wins 1075:1925 (35.8333%) draws 0
moves 95601 average 31.867 invalid 0
wins per start square:
  110:0       0:123   130:0       0:132   132:0
    0:120     0:110     0:122     0:117     0:118
  114:0       0:111   118:0       0:115   117:0
    0:125     0:129     0:124     0:124     0:113
  109:0       0:141   126:0       0:101   119:0

depth 5:5  start side X  random first move yes
games 2380 wins 1725:655 (72.479%) draws 0
moves 69137 average 29.0492 invalid 0
wins per start square:
   96:0      96:0      96:0     107:0     106:0
   96:0       0:83      0:100   103:0      85:0
   83:0       0:86     95:0       0:89      0:98
   99:0       0:106     0:93     91:0      96:0
   87:0     116:0     104:0      75:0      94:0



board 5 4  depth 2:2
start-side X  fixed-first-move yes
games 25  wins 25:0 (100%)  draws 0
moves 507  average 20.28  invalid 0
wins per start square:
    1:0       1:0       1:0       1:0       1:0
    1:0       1:0       1:0       1:0       1:0
    1:0       1:0       1:0       1:0       1:0
    1:0       1:0       1:0       1:0       1:0
    1:0       1:0       1:0       1:0       1:0

board 5 4  depth 3:3
start-side X  fixed-first-move yes
games 25  wins 7:18 (28%)  draws 0
moves 519  average 20.76  invalid 0
wins per start square:
    0:1       0:1       1:0       0:1       1:0
    0:1       0:1       0:1       0:1       0:1
    1:0       0:1       1:0       0:1       1:0
    0:1       0:1       0:1       0:1       0:1
    0:1       0:1       1:0       0:1       1:0

board 5 4  depth 4:4
start-side X  fixed-first-move yes
games 25  wins 9:16 (36%)  draws 0
moves 795  average 31.8  invalid 0
wins per start square:
    1:0       0:1       1:0       0:1       1:0
    0:1       0:1       0:1       0:1       0:1
    1:0       0:1       1:0       0:1       1:0
    0:1       0:1       0:1       0:1       0:1
    1:0       0:1       1:0       0:1       1:0

board 5 4  depth 5:5
start-side X  fixed-first-move yes
games 25  wins 18:7 (72%)  draws 0
moves 728  average 29.12  invalid 0
wins per start square:
    1:0       1:0       1:0       1:0       1:0
    1:0       0:1       0:1       1:0       1:0
    1:0       0:1       1:0       0:1       0:1
    1:0       0:1       0:1       1:0       1:0
    1:0       1:0       1:0       1:0       1:0

board 5 4  depth 6:6
start-side X  fixed-first-move yes
games 25  wins 3:16 (15.7895%)  draws 0
moves 3719  average 148.76  invalid 0
wins per start square:
    1:0       0:1       0:1       0:1       0:1
    0:1       0:1       0:0       0:1       0:0
    0:1       0:0       1:0       0:0       0:1
    0:1       0:1       0:0       0:1       0:0
    1:0       0:1       0:1       0:1       0:1




board 5 4  depth 2:4
start-side X  fixed-first-move yes
games 25  wins 6:16 (27.2727%)  draws 3
moves 732  average 29.28  invalid 0
wins per start square:
     :         :1       1:         :1        :1
     :1        :1        :1        :1        :1
    1:         :1       1:         :1       1:
     :1        :1        :1        :1       1:
     :         :1       1:         :1        :

board 5 4  depth 3:4
start-side X  fixed-first-move yes
games 25  wins 3:22 (12%)  draws 0
moves 617  average 24.68  invalid 0
wins per start square:
     :1        :1       1:         :1        :1
     :1        :1        :1        :1        :1
    1:         :1       1:         :1        :1
     :1        :1        :1        :1        :1
     :1        :1        :1        :1        :1

board 5 4  depth 4:4
start-side X  fixed-first-move yes
games 25  wins 9:16 (36%)  draws 0
moves 795  average 31.8  invalid 0
wins per start square:
    1:         :1       1:         :1       1:
     :1        :1        :1        :1        :1
    1:         :1       1:         :1       1:
     :1        :1        :1        :1        :1
    1:         :1       1:         :1       1:

board 5 4  depth 5:4
start-side X  fixed-first-move yes
games 25  wins 21:4 (84%)  draws 0
moves 645  average 25.8  invalid 0
wins per start square:
    1:        1:        1:        1:        1:
    1:        1:         :1       1:        1:
    1:         :1       1:         :1       1:
    1:        1:         :1       1:        1:
    1:        1:        1:        1:        1:

board 5 4  depth 6:4
start-side X  fixed-first-move yes
games 25  wins 17:8 (68%)  draws 0
moves 751  average 30.04  invalid 0
wins per start square:
    1:         :1       1:         :1       1:
     :1       1:        1:        1:         :1
    1:        1:        1:        1:        1:
     :1       1:        1:        1:         :1
    1:         :1       1:         :1       1:

**/
