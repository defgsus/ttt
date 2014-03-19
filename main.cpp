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

#include <iomanip>



int main(int , char **)
{
    std::cout << "Hello, you play X\n"
              << "Enter moves like: 'a1', 'c2' ...\n"
              << "I also understand 'quit', 'start', 'back', 'print', 'guess', 'tree' and 'btree'\n"
              << std::endl;

    int mdepth = 2;
    bool print_eval = true;
    Board b(5,4);
    Search ai;

    b.init();
    //std::cout << b.eval(X) << "\n"; return 0;

    std::vector<Board> stack;

    while (true)
    {
    reprint_:

        std::cout << std::endl;
        b.printBoard(print_eval);

        int evalx = b.eval(X),
            evalo = b.eval(O),
            eval = b.eval();
        std::cout << std::setw(b.size()*8+10) << "eval " << eval << " (X=" << evalx << " Y=" << evalo << ")" << std::endl;

    ask_:
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
            goto ask_;
        }
        else if (str == "btree")
        {
            ai.printTree(true);
            goto ask_;
        }
        else if (str == "guess")
        {
            int score;
            Move m = ai.bestMove(b, mdepth, &score);
            std::cout << "best move: " << b.toString(m) << " (" << score << ")" << std::endl;
            goto ask_;
        }
        else if (str == "back")
        {
            if (stack.size())
            {
                b = stack.back();
                stack.pop_back();
            }
            goto reprint_;
        }
        else if (str == "depth")
        {
            std::cin >> mdepth;
            goto ask_;
        }

        // parse move
        Move m = b.parseMove(str);
        if (m == InvalidMove)
        {
            std::cout << "invalid move, try again" << std::endl;
            goto ask_;
        }


        stack.push_back(b);

        // apply user move
        b.makeMove(m);
        b.flipStm();

        // check for win
        if (b.eval(X) >= MaxScore)
        {
            std::cout << "\nYou win!" << std::endl;
            goto reprint_;
        }

        // check for draw
        if (b.pieces() >= b.size() * b.size() - 1)
        {
            std::cout << std::endl;
            b.printBoard(print_eval);
            std::cout << "\nDraw! I wasn't really trying, though" << std::endl;
            std::cout << "final score " << b.eval(X) << ":" << b.eval(O) << std::endl;
            goto ask_;
        }

        // run ai
        int score;
        m = ai.bestMove(b, mdepth, &score);

        // ai is clueless ???
        if (m == InvalidMove)
        {
            std::cout << "I'm lost, you win!\n" << std::endl;
        }
        else
        {
            std::cout << "\nai move: " << b.toString(m) << " (" << score << ")" << std::endl;
            // apply ai move
            b.makeMove(m);
            b.flipStm();
        }

        if (b.eval(O) >= MaxScore)
        {
            std::cout << "\nI win, i'm a machine!\n" << std::endl;
            goto reprint_;
        }

    }

    haveit_:

    std::cout << "\nthank you\n" << std::endl;
}
