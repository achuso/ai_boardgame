#ifndef MINIMAX_H
#define MINIMAX_H

#include <vector>
#include <limits>
#include "TurnManager.h"

class Minimax {
private:
    int maxDepth;

    struct Move {
        int fromRow, fromCol;
        int toRow, toCol;
    };

    int evaluateBoard(const Board& board);
    int minMax(TurnManager turnManager, int depth, bool maximizingPlayer, int alpha, int beta); 
    std::vector<Move> generateMoves(const TurnManager& turnManager, int player);

public:
    Minimax(int depth);
    std::vector<Move> findBestMoves(TurnManager& turnManager);
};

#endif
