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

void printRules(Board b)
{
    b.init("XO.");
    b.makeMove(2);
    std::cout << "\n - X in a row in a N^2 - \n"
              << "-----------------------------\n"
              << "try to connect " << b.consecutives() << " of your pieces (X).\n"
#ifdef TTT_CAPTURE
              << "surround the other player to capture/exchange.\n"
              << "placing an X at the empty square\n"
              << "\n X O . \n"
              << "\ncreates\n"
              << "\n "<<pieceChar[b.pieceAt(0)]<<" "<<pieceChar[b.pieceAt(1)]<<" "<<pieceChar[b.pieceAt(2)]<<" \n"

#endif
              << std::endl;
}

void printHelp(bool shrt = false)
{
    std::cout << "\nHello Seeker\n"
              << "Enter moves like: 'a1', 'c2' ...\n"
              << "I also understand 'quit', 'start', 'back', 'print',\n"
              << "'X', 'O', 'moves', 'guess', 'play x', 'depth x', 'tree' and 'btree',\n"
              << "'size x x', 'help' and 'rules'\n"
              << std::endl;
    if (shrt) return;
    std::cout << "'x' and 'o' selects the player.\n"
              << "'play x' starts playing x two-plys\n"
              << "'size s n' sets the board to s by s squares and \n"
              << " the number of pieces, to be connected, to n.\n"
              << "'tree' shows previous search nodes\n"
              << " the max. displayed depth can be set with 'tree1' to 'tree9'\n\n"
              << "'btree' shows only the winner branch.\n"
#ifndef TTT_KEEP_TREE
              << " /* note, due to compile switch, only the first level is accessible */\n"
#endif
              << "\n"
              << std::endl;

}

int main(int , char **)
{/*
    .. .x x.
    .. x. .x
    0  1  2*/

    //std::cout << (2^3) << std::endl; return 0;

    printHelp(true);

    int mdepth = 2;
    bool print_eval = true;
    int autoplay = 0;

    Board b(3,3);
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
        std::cout << std::setw(b.size()*8+10) << "eval " << eval
                  << " (X=" << evalx << " Y=" << evalo << ")" << std::endl;

    ask_:
        std::string str;
        if (autoplay<=0)
        {
            std::cout << pieceChar[b.stm()] << ">";
            std::cin >> str;
        }

        // check command
        if (str == "q" || str == "quit" || str == "exit")
            goto haveit_;

        else if (str == "start")
        {
            b.init();
            goto reprint_;
        }
        else if (str == "play")
        {
            std::cin >> autoplay;
            if (!autoplay) goto ask_;
        }
        else if (str == "p" || str == "print")
            goto reprint_;

        else if (str == "help" || str == "?")
        {
            printHelp();
            goto ask_;
        }
        else if (str.find("rule")==0)
        {
            printRules(b);
            goto ask_;
        }
        else if (str == "size")
        {
            int s, c;
            std::cin >> s >> c;
            b.setSize(s,c);
            goto reprint_;
        }
        else if (str == "X" || str == "x")
        {
            if (b.stm() == O)
                b.flipStm();
            goto ask_;
        }
        else if (str == "O" || str == "o")
        {
            if (b.stm() == X)
                b.flipStm();
            goto ask_;
        }
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
        else if (str == "moves")
        {
            Moves m;
            b.getMoves(m);
            for (auto i : m)
                std::cout << " " << b.toString(i);
            std::cout << std::endl;
            goto ask_;
        }
        else if (str == "guess")
        {
            int score;
            Move m = ai.bestMove(b, mdepth, &score);
            std::cout << "best move: " << b.toString(m) << " (" << score << ")" << std::endl;
            goto ask_;
        }
        else if (str == "b" || str == "back")
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

        // ------ expect move here -----

        Move m = InvalidMove;

        if (!autoplay)
        {
            // parse move
            m = b.parseMove(str);
            if (m == InvalidMove)
            {
                std::cout << "invalid move, try again" << std::endl;
                goto ask_;
            }
        }
        else
        {
            // figure ai move
            int score;
            m = ai.bestMove(b, mdepth, &score);
            if (m == InvalidMove)
            {
                autoplay = 0;
                goto ask_;
            }
            std::cout << "best move: " << b.toString(m) << " (" << score << ")\n" << std::endl;
        }

        // store history
        stack.push_back(b);

        // apply user move
        b.makeMove(m);
        std::cout << std::endl;
        b.printBoard(true);
        std::cout << std::endl;
        b.flipStm();

        if (autoplay>0)
        {
            b.printBoard(true);
            std::cout << std::endl;
            autoplay--;
        }

        if (true)
        {
            // check for win
            if (b.isWin(X))
            {
                std::cout << "\nYou win!" << std::endl;
                autoplay = 0;
                b.flipStm();
                goto reprint_;
            }

            if (b.isDraw())
            {
                b.flipStm();
                goto draw_;
            }

            // run ai
            int score;
            m = ai.bestMove(b, mdepth, &score);

            // ai is clueless ???
            if (m == InvalidMove)
            {
                std::cout << "I'm lost!\n" << std::endl;
                autoplay = 0;
                goto ask_;
            }

            // -- apply ai move --

            std::cout << "ai move: " << b.toString(m) << " (" << score << ")" << std::endl;
            b.makeMove(m);
            b.flipStm();

            if (b.isWin(O))
            {
                std::cout << "\nI win, i'm a machine!\n" << std::endl;
                autoplay = 0;
                goto reprint_;
            }

            if (b.isDraw())
                goto draw_;
        }
        goto reprint_;

    draw_:
        std::cout << std::endl;
        if (!autoplay) b.printBoard(print_eval);
        std::cout << "\nDraw! I wasn't really trying, though" << std::endl;
        std::cout << "final score " << b.eval(X) << ":" << b.eval(O) << std::endl;
        autoplay = 0;
        goto ask_;

    }

    haveit_:

    std::cout << "\nthank you\n" << std::endl;
}
