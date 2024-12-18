#include "Minimax.h"
#include <limits>

Minimax::Minimax(int depth) : maxDepth(depth) {}

int Minimax::evaluateBoard(const Board& board) {
    int p1Count = 0, p2Count = 0;

    for (const auto& row : board.getBoard()) {
        for (int cell : row) {
            if (cell == P1_PIECE) 
                p1Count++;
            else if (cell == P2_PIECE) 
                p2Count++;
        }
    }

    return p1Count - p2Count; // primitive heuristic fn, experiment later
}

int Minimax::minMax(Board board, int depth, bool maximizingPlayer) {
    if (depth == 0 || board.checkGameEnd()) {
        return evaluateBoard(board);
    }

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();

        for (const auto& move : generateMoves(board, P1_PIECE)) {
            Board newBoard = board;
            newBoard.movePiece(P1_PIECE, move.fromRow, move.fromCol, move.toRow, move.toCol);
            int eval = minMax(newBoard, depth - 1, false);
            maxEval = std::max(maxEval, eval);
        }

        return maxEval;
    } 
    else {
        int minEval = std::numeric_limits<int>::max();

        for (const auto& move : generateMoves(board, P2_PIECE)) {
            Board newBoard = board;
            newBoard.movePiece(P2_PIECE, move.fromRow, move.fromCol, move.toRow, move.toCol);
            int eval = minMax(newBoard, depth - 1, true);
            minEval = std::min(minEval, eval);
        }

        return minEval;
    }
}

std::vector<Minimax::Move> Minimax::generateMoves(const Board& board, int player) {
    std::vector<Move> moves;

    for (int row = 0; row < 7; ++row) {
        for (int col = 0; col < 7; ++col) {
            if (board.getPiece(row, col) == player) {
                for (int dRow = -1; dRow <= 1; ++dRow) {
                    for (int dCol = -1; dCol <= 1; ++dCol) {
                        if ((dRow == 0 || dCol == 0) && board.isValidMove(player, row, col, row + dRow, col + dCol)) {
                            moves.push_back({row, col, row + dRow, col + dCol});
                        }
                    }
                }
            }
        }
    }

    return moves;
}

// best move for AI
Minimax::Move Minimax::findBestMove(const Board& board) {
    int bestValue = std::numeric_limits<int>::min();
    Move bestMove = {0, 0, 0, 0};

    for (const auto& move : generateMoves(board, P1_PIECE)) {
        Board newBoard = board;
        newBoard.movePiece(P1_PIECE, move.fromRow, move.fromCol, move.toRow, move.toCol);

        int moveValue = minMax(newBoard, maxDepth, false);
        if (moveValue > bestValue) {
            bestValue = moveValue;
            bestMove = move;
        }
    }

    return bestMove;
}
