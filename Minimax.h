#ifndef MINIMAX_H
#define MINIMAX_H

#include <vector>
#include "Board.h"

class Minimax {
private:
    int maxDepth;

    struct Move {
        int fromRow, fromCol;
        int toRow, toCol;
    };

    int evaluateBoard(const Board& board); // evaluate board and return heuristic value
    int minMax(Board board, int depth, bool maximizingPlayer); // recursive minimax algo
    std::vector<Move> generateMoves(const Board& board, int player); // generate all possible moves

public:
    Minimax(int depth = 3);
    Move findBestMove(const Board& board); // best move for AI agent
};

#endif
