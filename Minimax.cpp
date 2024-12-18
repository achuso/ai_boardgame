#include "Minimax.h"

Minimax::Minimax(int depth) : maxDepth(depth) {}

const Board& TurnManager::getBoard() const {
    return board;
}

 // heuristic fn.
int Minimax::evaluateBoard(const Board& board) {
    /*
    Criteria to consider in the future:
        1) Is a piece accompanied by an ally? (Reduces odds of being sandwiched)
        2) Does being on the edge provide an opportunity to capture a piece?
        3) Does a capture ensure safety?
    */
    int p1Score = 0, p2Score = 0;

    // directions for capturing logic
    std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int piece = board.getPiece(row, col);

            if (piece == P1_PIECE || piece == P2_PIECE) {
                int playerScore = (piece == P1_PIECE) ? p1Score : p2Score;

                // base score for having a piece
                playerScore += 10;

                // reward center positions like magnus carlssen destroying super GM's
                if (row >= 2 && row <= 4 && col >= 2 && col <= 4) playerScore += 5;

                // punish edge positions. magnus never makes mistakes.
                if (row == 0 || row == 6 || col == 0 || col == 6) playerScore -= 3;

                // check for capture potential
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

                if (piece == P1_PIECE)
                    p1Score = playerScore;
                else
                    p2Score = playerScore;
            }
        }
    }

    return p1Score - p2Score;
}

std::vector<Minimax::Move> Minimax::generateMoves(const TurnManager& turnManager, int player) {
    std::vector<Move> moves;

    const Board& board = turnManager.getBoard();
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (board.getPiece(row, col) == player) {
                std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
                for (auto [dRow, dCol] : directions) {
                    int newRow = row + dRow, newCol = col + dCol;
                    if (board.isValidMove(player, row, col, newRow, newCol)) {
                        moves.push_back({row, col, newRow, newCol});
                    }
                }
            }
        }
    }

    return moves;
}

int Minimax::minMax(TurnManager& turnManager, int depth, bool maximizingPlayer, int alpha, int beta) {
    Board::GameResult result = turnManager.getGameState();

    if (depth == 0 || result != Board::GameResult::Ongoing)
        return evaluateBoard(turnManager.getBoard());

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        auto moves = generateMoves(turnManager, P1_PIECE);

        for (const auto& move : moves) {
            TurnManager tempManager = turnManager;
            tempManager.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol);

            int eval = minMax(tempManager, depth - 1, false, alpha, beta);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);

            if (beta <= alpha) break; // alpha-beta pruning
        }

        return maxEval;
    } 
    else {
        int minEval = std::numeric_limits<int>::max();
        auto moves = generateMoves(turnManager, P2_PIECE);

        for (const auto& move : moves) {
            TurnManager tempManager = turnManager;
            tempManager.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol);

            int eval = minMax(tempManager, depth - 1, true, alpha, beta);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);

            if (beta <= alpha) break; // alpha-beta pruning
        }

        return minEval;
    }
}

std::vector<Minimax::Move> Minimax::findBestMoves(TurnManager& turnManager) {
    std::vector<Move> bestMoves;
    int bestValue = std::numeric_limits<int>::min();
    Move bestMove = {0, 0, 0, 0};

    auto moves = generateMoves(turnManager, P1_PIECE);
    for (const auto& move : moves) {
        TurnManager tempManager = turnManager;
        tempManager.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol);

        int eval = minMax(tempManager, maxDepth, false, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        if (eval > bestValue) {
            bestValue = eval;
            bestMove = move;
        }
    }

    bestMoves.push_back(bestMove);
    return bestMoves;
}