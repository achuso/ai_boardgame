#include "Minimax.h"
#include <iostream>
#include <cmath>

Minimax::Minimax(int depth) : maxDepth(depth) {}

// evaluate the board for scoring
int Minimax::evaluateBoard(const Board& board) {
    int p1Score = 0, p2Score = 0;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int piece = board.getPiece(row, col);
            if (piece == P1_PIECE) {
                p1Score += 10;
                if (row >= 2 && row <= 4 && col >= 2 && col <= 4)
                    p1Score += 6; // center bonus, nerf the modern defense
                if (row == 0 || row == 6 || col == 0 || col == 6)
                        p1Score -= 2; // edge penalty (my AI would never)
            }
            else if (piece == P2_PIECE) {
                p2Score += 10;
                if (row >= 2 && row <= 4 && col >= 2 && col <= 4)
                    p2Score += 6; // center bonus, but I ain't no Magnus Carlssen so this is redundant
                if (row == 0 || row == 6 || col == 0 || col == 6)
                    p2Score -= 2; // edge penalty I will gladly receive
            }
        }
    }

    return p1Score - p2Score;
}

// generates all valid moves for the given player
std::vector<Move> Minimax::generateMoves(const TurnManager& turnManager, int player) {
    const Board* board = turnManager.getBoard();
    std::vector<Move> moves;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (board->getPiece(row, col) == player) {
                for (auto [dRow, dCol] : {std::make_pair(1, 0), {-1, 0}, {0, 1}, {0, -1}}) {
                    int newRow = row + dRow;
                    int newCol = col + dCol;

                    if (board->isValidMove(player, row, col, newRow, newCol)) {
                        moves.push_back({row, col, newRow, newCol});
                    }
                }
            }
        }
    }

    return moves;
}

// minimax algorithm w alpha-beta pruning
int Minimax::minMax(const TurnManager& turnManager, int depth, bool maximizingPlayer, int alpha, int beta) {
    Board::GameResult result = turnManager.getGameState();

    if (depth == 0 || result != Board::GameResult::Ongoing) {
        return evaluateBoard(*turnManager.getBoard());
    }

    int bestValue = maximizingPlayer ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    auto moves = generateMoves(turnManager, turnManager.getCurrentPlayer());

    for (const auto& move : moves) {
        TurnManager tempManager = turnManager;
        if (tempManager.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol) == TurnManager::MoveResult::Success) {
            if (tempManager.isMoveLimitExceeded()) {
                tempManager.endTurn();
            }

            int eval = minMax(tempManager, depth - 1, !maximizingPlayer, alpha, beta);
            if (maximizingPlayer) {
                bestValue = std::max(bestValue, eval);
                alpha = std::max(alpha, eval);
            }
            else {
                bestValue = std::min(bestValue, eval);
                beta = std::min(beta, eval);
            }

            if (beta <= alpha) {
                break; // a-b pruning
            }
        }
    }

    return bestValue;
}

// find the best moves for the current player
std::vector<Move> Minimax::findBestMoves(TurnManager& turnManager) {
    std::vector<Move> bestMoves;
    int bestValue = std::numeric_limits<int>::min();

    auto moves = generateMoves(turnManager, turnManager.getCurrentPlayer());
    for (const auto& move : moves) {
        TurnManager tempManager = turnManager;
        if (tempManager.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol) == TurnManager::MoveResult::Success) {
            if (tempManager.isMoveLimitExceeded()) {
                tempManager.endTurn();
            }

            int eval = minMax(tempManager, maxDepth - 1, false, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
            if (eval > bestValue) {
                bestValue = eval;
                bestMoves.clear();
                bestMoves.push_back(move);
            }
            else if (eval == bestValue) {
                bestMoves.push_back(move);
            }
        }
    }

    return bestMoves;
}