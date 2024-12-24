#include "Minimax.h"
#include <iostream>
#include <cmath>

Minimax::Minimax(int depth) : maxDepth(depth) {}

// evaluate the board for scoring
int Minimax::evaluateBoard(const Board& board) {
    int p1Score = 0, p2Score = 0;

    int centerStart = 2, centerEnd = 4;      // define center bounds
    int centerBonus = 4;                    // bonus for center control
    int edgePenalty = -2;                    // edge penalty
    int captureBonus = 10;                   // bonus for captures
    int pieceBonus = 8;                      // base score for each piece
    int aggressionBonus = 15;                // aggressive moves when opponents are short on pieces

    int p1_pieceCount = 0, p2_pieceCount = 0;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int piece = board.getPiece(row, col);

            if (piece == P1_PIECE) {
                p1_pieceCount++;
                p1Score += pieceBonus;
                // ♬ i'll make a man out of you ♬
                if (row >= centerStart && row <= centerEnd && col >= centerStart && col <= centerEnd)
                    p1Score += centerBonus;
                // edge penalty (my AI would never)
                if (row == 0 || row == BOARD_SIZE - 1 || col == 0 || col == BOARD_SIZE - 1)
                    p1Score += edgePenalty;
            }
            else if (piece == P2_PIECE) {
                p2_pieceCount++;
                p2Score += pieceBonus;
                // center bonus for me, redundant
                if (row >= centerStart && row <= centerEnd && col >= centerStart && col <= centerEnd)
                    p2Score += centerBonus;
                // edge penalty I will gladly receive
                if (row == 0 || row == BOARD_SIZE - 1 || col == 0 || col == BOARD_SIZE - 1)
                    p2Score += edgePenalty;
            }
        }
    }

    // aggression weights
    if (p2_pieceCount < (p1_pieceCount / 2))
        p1Score += aggressionBonus;
    if (p1_pieceCount < (p2_pieceCount / 2))
        p2Score += aggressionBonus;

    // encouragement for captures
    int piece_diff = (p1_pieceCount - p2_pieceCount) * captureBonus;
    p1Score += piece_diff;
    p2Score -= piece_diff;

    return p1Score - p2Score;
}

// generates all valid moves for the given player
std::vector<Move> Minimax::generateMoves(const TurnManager& turnManager, int player) {
    const Board* board = turnManager.getBoard();
    std::vector<Move> moves, capturingMoves;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (board->getPiece(row, col) == player) {
                for (auto [dRow, dCol] : {std::make_pair(1, 0), {-1, 0}, {0, 1}, {0, -1}}) {
                    int newRow = row + dRow;
                    int newCol = col + dCol;

                    if (board->isValidMove(player, row, col, newRow, newCol)) {
                        Move move = {row, col, newRow, newCol};

                        // simulate the move to check if it results in a capture
                        TurnManager tempManager = turnManager;
                        if (tempManager.makeMove(row, col, newRow, newCol) == TurnManager::MoveResult::Success) {
                            int opponentPieceCountBefore = board->countPieces(player == P1_PIECE ? P2_PIECE : P1_PIECE);
                            int opponentPieceCountAfter = tempManager.getBoard()->countPieces(player == P1_PIECE ? P2_PIECE : P1_PIECE);

                            if (opponentPieceCountAfter < opponentPieceCountBefore) {
                                capturingMoves.push_back(move); // prioritize capturing moves
                            }
                            else {
                                moves.push_back(move); // non-capturing moves
                            }
                        }
                    }
                }
            }
        }
    }

    capturingMoves.insert(capturingMoves.end(), moves.begin(), moves.end());
    return capturingMoves;
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