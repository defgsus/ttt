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
#include "negamax.h"

#include <iomanip>

using namespace TTT;





void printRules(Board b)
{
    // prepare board for rules
    std::string bs = "X";
    for (int i=0; i<(int)b.size()-2; ++i)
        bs += "O";
    bs += ".";
    b.init(bs);

    std::cout << "\n - X in a row in N^2 - \n"
              << "---------------------------\n"
              << "try to connect " << b.consecutives() << " of your pieces (X).\n";
#ifdef TTT_CAPTURE
    std::cout << "surround the other player to capture/exchange.\n"
              << "e.g. placing an X at the empty square\n";
    for (size_t i=0; i<b.size(); ++i)
        std::cout << " " << b.pieceCharAt(i);
    std::cout << "\n\ncreates\n\n";
    b.makeMove(b.size()-1);
    for (size_t i=0; i<b.size(); ++i)
        std::cout << " " << b.pieceCharAt(i);
#ifdef TTT_CAPTURE_WAIT
    std::cout << "\n\nwhile the " << b.pieceCharAt(1) << " square is blocked for two plies.";
#endif

#endif
    std::cout << "\n" << std::endl;
}

void printHelp(bool shrt = false)
{
    std::cout << "\nHello Seeker\n"
              << "Enter moves like: 'a1', 'c2' ...\n"
              << "I also understand 'quit', 'start', 'back', 'print',\n"
              << "'X', 'O', 'moves', 'guess', 'play x', 'depth x', 'tree' and 'btree',\n"
              << "'size x x', "
#ifdef TTT_GREEDY
              << "'greed x', "
#endif
              << "'help' and 'rules'\n"
              << std::endl;
    if (shrt) return;

    std::cout << "'x' and 'o' selects the player.\n"
              << "'play n' starts playing n moves\n"
              << "'size s n' sets the board to s by s squares and \n"
              << " the number of pieces, to be connected, to n.\n"
#ifdef TTT_GREEDY
              << "\n'greed x' sets my greed value. a positive value leads me\n"
              << " to consider child nodes with less improvement unworthy.\n"
              << " a negative value cuts off nodes that declined more than this.\n"
#endif

#ifdef TTT_KEEP_TREE
              << "/* not implemented */\n"
              << "\n'tree' shows previous search nodes\n"
              << " the max. displayed depth can be set with 'tree1' to 'tree9'\n"
              << "'btree' shows only the winner branch.\n"
#endif
              << "\n"
              << std::endl;

}

void test()
{
    Board b(3,3);
    Board * b2 = new Board(b),
          * b3 = new Board(*b2);
    delete b2;
    delete b3;
}

void test_engines();

int main(int , char **)
{
    test_engines(); return 0;

    printHelp(true);

    int mdepth = 2 << 1;
    bool print_eval = true;
    int autoplay = 0;

    Board b(5,4);
    BoardHelper bh(b);
    Search ai;

    b.init();
    //b.init("O.O.XO..XXXo..XXXxXOXXXOX");
    //b.init("X..XX.o.....o....OXXX..XX");
    //b.init("O.O.XO..XXX...XXX.XOXXXOX");
    //std::cout << b.eval(X) << "\n"; return 0;

    std::vector<Board> stack;

    while (true)
    {
    reprint_:

        std::cout << std::endl;
        b.printBoard(print_eval);

        int eval = bh.eval(b);
        std::cout << std::setw(b.size()*8+10) << ""
                  << " eval " << eval
#ifdef TTT_CAPTURE
                  << " caps " << b.numCaptures() << " (" << b.numCapturablePieces() << ")"
#endif
                  << std::endl;

    ask_:

        // ask for input or autoplay
        std::string str;
        if (autoplay<=0)
        {
            std::cout << Board::pieceChar(b.stm()) << ">";
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
            bh.setSize(b);
            stack.clear();
            goto reprint_;
        }
#ifdef TTT_GREEDY
        else if (str.find("greed")==0)
        {
            int g;
            std::cin >> g;
            ai.greed = g;
            goto ask_;
        }
#endif
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
        /*
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
            //ai.printTree(true);
            goto ask_;
        }
        */
        else if (str == "moves")
        {
            Moves m;
            bh.getMoves(b, m);
            for (auto i : m)
                std::cout << " " << b.toString(i);
            std::cout << std::endl;
            goto ask_;
        }
        else if (str == "guess")
        {
            Move m = ai.bestMove(b, mdepth);
            std::cout << "best move: " << b.toString(m) << " (" << ai.score << ")" << std::endl;
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

        if (bh.isOver(b))
        {
            std::cout << "the game is over, buddy" << std::endl;
            autoplay = 0;
            goto ask_;
        }

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
            // figure user's ai move
            m = ai.bestMove(b, mdepth);
            if (m == InvalidMove)
            {
                autoplay = 0;
                goto ask_;
            }
            std::cout << "best move: " << b.toString(m) << " (" << ai.score << ")\n" << std::endl;
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

        //goto ask_;

        if (true)
        {
            // check for win
            if (bh.isWin(b, X))
            {
                std::cout << "\n--------"
                             "\nYou win!"
                             "\n--------\n" << std::endl;
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
            m = ai.bestMove(b, mdepth);

            // ai is clueless ???
            if (m == InvalidMove)
            {
                std::cout << "I'm lost!\n" << std::endl;
                autoplay = 0;
                goto ask_;
            }

            // -- apply ai move --

            std::cout << "ai move: " << b.toString(m) << " (" << ai.score << ")" << std::endl;
            b.makeMove(m);
            b.flipStm();

            if (bh.isWin(b, O))
            {
                std::cout << "\n---------------------"
                             "\nI win, i'm a machine!"
                             "\n---------------------\n" << std::endl;
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
        std::cout << "\n------------------------------------"
                     "\nDraw! I wasn't really trying, though"
                     "\n------------------------------------" << std::endl;
        std::cout << "final score " << bh.eval(b) << std::endl;
        autoplay = 0;
        goto ask_;

    }

    haveit_:

    std::cout << "\nthank you\n" << std::endl;
}
