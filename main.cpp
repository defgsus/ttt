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
              << "I also understand 'quit' and 'start'\n"
              << std::endl;

    Board b;
    Search ai;

    b.init();

    while (true)
    {
    reprint_:

        b.printBoard();

        int evalx = b.eval(X),
            evalo = b.eval(O);

        std::cout << "              score X=" << evalx << " O=" << evalo << std::endl;

        if (evalx >= 1000)
        {
            std::cout << "You win!\n" << std::endl;
            b.init();
            goto reprint_;
        }

        if (evalo >= 1000)
        {
            std::cout << "I win, i'm a machine!\n" << std::endl;
            b.init();
            goto reprint_;
        }

    again_:
        std::cout << ">";
        std::string str;
        std::cin >> str;
        std::cout << std::endl;

        if (str == "q" || str == "quit")
            goto haveit_;
        else if (str == "start")
        {
            b.init();
            goto reprint_;
        }

        Move m = b.parseMove(str);
        if (m == InvalidMove)
        {
            std::cout << "invalid move, try again" << std::endl;
            goto again_;
        }

        b.makeMove(m);

        b.flipStm();

        m = ai.bestMove(b);

        if (m == InvalidMove)
        {
            std::cout << "I'm lost, you win!\n" << std::endl;
            b.init();
        }
        else
        {
            b.makeMove(m);
            b.flipStm();
        }
    }

    haveit_:

    std::cout << "\nthank you\n" << std::endl;
}
