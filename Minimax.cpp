#include "Minimax.h"
#include <iostream>
#include <cmath>

Minimax::Minimax(int depth) : maxDepth(depth) {}

int Minimax::evaluateBoard(const Board& board) {
    int p1Score = 0, p2Score = 0;
    std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int piece = board.getPiece(row, col);

            if (piece == P1_PIECE || piece == P2_PIECE) {
                int& playerScore = (piece == P1_PIECE) ? p1Score : p2Score;

                // base score for having a piece
                playerScore += 10;

                // center bonus
                if (row >= 2 && row <= 4 && col >= 2 && col <= 4) playerScore += 5;
                // edge penalty
                if (row == 0 || row == 6 || col == 0 || col == 6) playerScore -= 3;

                // check capture potential
                for (auto [dRow, dCol] : directions) {
                    int adjRow = row + dRow, adjCol = col + dCol;
                    int beyondRow = adjRow + dRow, beyondCol = adjCol + dCol;

                    if (board.inBounds(adjRow, adjCol) && board.inBounds(beyondRow, beyondCol)) {
                        int adjacentPiece = board.getPiece(adjRow, adjCol);
                        int beyondPiece = board.getPiece(beyondRow, beyondCol);

                        if (adjacentPiece != piece && adjacentPiece != EMPTY) {
                            if (beyondPiece == piece) {
                                // potential to capture an opponent piece
                                playerScore += 15;
                            } 
                            else if (beyondPiece == EMPTY) {
                                // threatened but not captured yet
                                playerScore += 5;
                            }
                        }
                    }
                }
            }
        }
    }

    return p1Score - p2Score;
}

std::vector<Minimax::Move> Minimax::generateMoves(const TurnManager& turnManager, int player) {
    const Board* board = turnManager.getBoard();
    std::vector<Move> moves;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (board->getPiece(row, col) == player) {
                std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

                for (auto [dRow, dCol] : directions) {
                    int newRow = row + dRow;
                    int newCol = col + dCol;
                    if (board->isValidMove(player, row, col, newRow, newCol)) {
                        moves.push_back({row, col, newRow, newCol});
                    } 
                    // else {
                    //     std::cout << "Invalid move: (" << row << ", " << col << ") -> (" << newRow << ", " << newCol << ")\n";
                    // }
                }
            }
        }
    }

    return moves;
}

int Minimax::minMax(TurnManager turnManager, int depth, bool maximizingPlayer, int alpha, int beta) {
    Board::GameResult result = turnManager.getGameState();

    if (depth == 0 || result != Board::GameResult::Ongoing)
        return evaluateBoard(*turnManager.getBoard());

    int currentPlayer = turnManager.getCurrentPlayer(); // get the current player
    std::vector<Move> moves = generateMoves(turnManager, currentPlayer);

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();

        for (const auto& move : moves) {
            TurnManager tempManager = turnManager; // copies board and state
            // attempt this move
            if (tempManager.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol) == TurnManager::MoveResult::Success) {
                // if the move limit is exceeded, end turn
                if (tempManager.isMoveLimitExceeded()) {
                    tempManager.endTurn();
                }
                int eval = minMax(tempManager, depth - 1, false, alpha, beta);
                maxEval = std::max(maxEval, eval);
                alpha = std::max(alpha, eval);
                if (beta <= alpha) break; // alpha-beta pruning!
            }
        }
        return maxEval;
    } 
    else {
        int minEval = std::numeric_limits<int>::max();

        for (const auto& move : moves) {
            TurnManager tempManager = turnManager;
            if (tempManager.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol) == TurnManager::MoveResult::Success) {
                if (tempManager.isMoveLimitExceeded()) {
                    tempManager.endTurn();
                }
                int eval = minMax(tempManager, depth - 1, true, alpha, beta);
                minEval = std::min(minEval, eval);
                beta = std::min(beta, eval);
                if (beta <= alpha) break;
            }
        }
        return minEval;
    }
}

std::vector<Minimax::Move> Minimax::findBestMoves(TurnManager& turnManager) {
    std::vector<Move> bestMoves;
    int bestValue = std::numeric_limits<int>::min();
    Move bestMove = {0, 0, 0, 0};

    int currentPlayer = turnManager.getCurrentPlayer();
    auto moves = generateMoves(turnManager, currentPlayer);

    for (const auto& move : moves) {
        TurnManager tempManager = turnManager; 
        if (tempManager.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol) == TurnManager::MoveResult::Success) {
            if (tempManager.isMoveLimitExceeded()) {
                tempManager.endTurn();
            }
            int eval = minMax(tempManager, maxDepth, currentPlayer == P1_PIECE, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
            if (eval > bestValue) {
                bestValue = eval;
                bestMove = move;
            }
        }
    }

    bestMoves.push_back(bestMove);
    return bestMoves;
}