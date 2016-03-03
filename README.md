# Reversi (Othello) Game 
Developed an Artificial Intelligence agent to play a game of othello against a human. The agent is developed in C++ and employs various heuristics concerned with game such as corner capture. The agent uses Minimax and Alpha Beta Pruning to explore game states.

Input
```
<task#> Greedy = 1, MiniMax = 2, Alpha-beta = 3
<your player: X or O>
<cutting off depth for Minimax or Alpha-beta>
<current state as follows:>
*: blank cell
X: Black player
O: White Player
Example:
2
X
2
********
********
********
***OX***
***XO***
********
********
********
```

Output
```
<next state>
Example:
********
********
***X****
***XX***
***XO***
********
********
********
```
