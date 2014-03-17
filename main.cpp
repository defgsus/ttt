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
#include "search.h"

int main(int , char **)
{
    std::cout << "Hello, you play X\n"
              << "Enter moves like: 'a1', 'c2' ...\n"
              << "I also understand 'quit', 'start', 'print', 'guess', 'tree' and 'btree'\n"
              << std::endl;

    Board b(3);
    Search ai;

    b.init();
    //std::cout << b.eval(X) << "\n"; return 0;

    while (true)
    {
    reprint_:

        std::cout << std::endl;
        b.printBoard();

        int evalx = b.eval(X),
            evalo = b.eval(O),
            eval = b.eval();
        std::cout << "              eval " << eval << " (X=" << evalx << " Y=" << evalo << ")" << std::endl;

        if (evalx >= MaxScore)
        {
            std::cout << "\nYou win!\n" << std::endl;
            b.init();
            goto reprint_;
        }

        if (evalo >= MaxScore)
        {
            std::cout << "\nI win, i'm a machine!\n" << std::endl;
            b.init();
            goto reprint_;
        }

    again_:
        std::cout << ">";
        std::string str;
        std::cin >> str;

        // check command
        if (str == "q" || str == "quit")
            goto haveit_;
        else if (str == "start")
        {
            b.init();
            goto reprint_;
        }
        else if (str == "print")
            goto reprint_;
        else if (str.find("tree") == 0)
        {
            int level = -1;
            if (str.size() >= 5)
                level = str[4] - '0';
            ai.printTree(false, level);
            goto again_;
        }
        else if (str == "btree")
        {
            ai.printTree(true);
            goto again_;
        }
        else if (str == "guess")
        {
            int score;
            Move m = ai.bestMove(b, &score);
            std::cout << "best move: " << b.toString(m) << " (" << score << ")" << std::endl;
            goto again_;
        }

        // parse move
        Move m = b.parseMove(str);
        if (m == InvalidMove)
        {
            std::cout << "invalid move, try again" << std::endl;
            goto again_;
        }

        // apply user move
        b.makeMove(m);
        b.flipStm();

        // check for win
        if (b.eval(X) >= MaxScore)
        {
            std::cout << "\nYou win!" << std::endl;
            b.init();
            goto reprint_;
        }

        // check for draw
        if (b.pieces() >= b.size() * b.size() - 1)
        {
            std::cout << std::endl;
            b.printBoard();
            std::cout << "\nDraw! I wasn't really trying, though" << std::endl;
            std::cout << "final score " << b.eval(X) << ":" << b.eval(O) << std::endl;
            b.init();
            goto reprint_;
        }

        // run ai
        int score;
        m = ai.bestMove(b, &score);

        // ai is clueless ???
        if (m == InvalidMove)
        {
            std::cout << "I'm lost, you win!" << std::endl;
            b.init();
        }
        else
        {
            std::cout << "\nai score: " << score << std::endl;
            // apply ai move
            b.makeMove(m);
            b.flipStm();
        }

    }

    haveit_:

    std::cout << "\nthank you\n" << std::endl;
}
