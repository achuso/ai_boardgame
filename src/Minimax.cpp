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
                int opponent = (piece == P1_PIECE) ? P2_PIECE : P1_PIECE;

                // base score for having a piece
                playerScore += 10;

                // center bonus
                if (row >= 2 && row <= 4 && col >= 2 && col <= 4)
                    playerScore += (p1Score <= p2Score ? 5 : 2);

                // edge penalty
                if (row == 0 || row == 6 || col == 0 || col == 6) 
                    playerScore -= 3;

                // check proximity to opponent pieces
                for (int prow = 0; prow < BOARD_SIZE; ++prow) {
                    for (int pcol = 0; pcol < BOARD_SIZE; ++pcol) {
                        if (board.getPiece(prow, pcol) == opponent) {
                            int distance = std::abs(prow - row) + std::abs(pcol - col);
                            playerScore -= distance; // penalize for being far from opponents
                        }
                    }
                }

                // check capture potential
                for (auto [dRow, dCol] : directions) {
                    int adjRow = row + dRow, adjCol = col + dCol;
                    int beyondRow = adjRow + dRow, beyondCol = adjCol + dCol;

                    if (board.inBounds(adjRow, adjCol) && board.inBounds(beyondRow, beyondCol)) {
                        int adjacentPiece = board.getPiece(adjRow, adjCol);
                        int beyondPiece = board.getPiece(beyondRow, beyondCol);

                        if (adjacentPiece != piece && adjacentPiece != EMPTY) {
                            if (beyondPiece == piece) {
                                playerScore += 15; // potential to capture
                            } 
                            else if (beyondPiece == EMPTY) {
                                playerScore += 5; // threatened but not captured yet
                            }
                        }
                    }
                }
            }
        }
    }

    // winning incentive
    if (p2Score == 1) 
        p1Score += 30; // encourage AI to capture the final piece

    return p1Score - p2Score;
}

std::vector<Minimax::Move> Minimax::generateMoves(const TurnManager& turnManager, int player) {
    const Board* board = turnManager.getBoard();
    std::vector<Move> moves;
    int opponent = (player == P1_PIECE) ? P2_PIECE : P1_PIECE;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (board->getPiece(row, col) == player) {
                std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

                for (auto [dRow, dCol] : directions) {
                    int newRow = row + dRow;
                    int newCol = col + dCol;

                    if (board->isValidMove(player, row, col, newRow, newCol)) {
                        Move move = {row, col, newRow, newCol};
                        // prioritize captures
                        if (board->getPiece(newRow, newCol) == opponent) {
                            moves.insert(moves.begin(), move); // capture moves first
                        } 
                        else {
                            moves.push_back(move);
                        }
                    }
                }
            }
        }
    }

    // sort moves by proximity to the opponent
    int opponentRow = -1, opponentCol = -1;
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (board->getPiece(row, col) == opponent) {
                opponentRow = row;
                opponentCol = col;
                break;
            }
        }
    }

    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        int distA = std::abs(a.toRow - opponentRow) + std::abs(a.toCol - opponentCol);
        int distB = std::abs(b.toRow - opponentRow) + std::abs(b.toCol - opponentCol);
        return distA < distB;
    });

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