#ifndef MINIMAX_H
#define MINIMAX_H

#include "TurnManager.h"
#include "GameConstants.h"
#include <vector>

class Minimax {
private:
    int maxDepth;

    // move generation and evaluation
    static int evaluateBoard(const Board& board);
    std::vector<Move> generateMoves(const TurnManager& turnManager, int player);
    // core ai logic
    int minMax(const TurnManager &turnManager, int depth, bool maximizingPlayer, int alpha, int beta);

public:
    explicit Minimax(int depth);

    // finds the best moves for the current player
    std::vector<Move> findBestMoves(TurnManager& turnManager);
};

#endif