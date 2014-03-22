TicTacToe With capturing - engine

This is a little project to learn about minimax and alpha-beta pruning.

Properties:
 - runs as interactive console app
 - runs in parallel, splitting at root nodes
 - optional greedy cut-off strategy 
 - includes Qt project (and uses the QTime object)
 - allows arbitrary baord and row sizes 
 - allows capturing, e.g. XOO. -> XOOX = X..X

DISCLAIMER:
The Engine plays okay i think - still quite slow (1-5 million nodes-per-sec on ubu64/i7).
Sometimes (with search depth >= 6 or so), the opponent will do some stupid stuff instead of breaking your winning row. This is because it already knows it has lost and tries to get at least a better evaluation value at the end ;)

TODO: 
 - Need time limit for moves...
