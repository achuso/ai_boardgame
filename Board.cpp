#include <iostream>
#include "Board.h"

Board::Board() {
    // init board
    for (auto& row : board) row.fill(EMPTY);

    // init player 1's pieces
    board[0][0] = P1_PIECE;
    board[2][0] = P1_PIECE;
    board[4][6] = P1_PIECE;
    board[6][6] = P1_PIECE;

    // init player 2's pieces
    board[0][6] = P2_PIECE;
    board[2][6] = P2_PIECE;
    board[4][0] = P2_PIECE;
    board[6][0] = P2_PIECE;
}

// void Board::printBoard() const {
//     std::cout << "Current Board State:" << std::endl;
//     for (int row = 0; row < BOARD_SIZE; ++row) {
//         for (int col = 0; col < BOARD_SIZE; ++col) {
//             int piece = board[row][col];
//             if (piece == EMPTY) {
//                 std::cout << ". "; // Represent empty cells
//             } else if (piece == P1_PIECE) {
//                 std::cout << "▲ "; // Player 1's pieces
//             } else if (piece == P2_PIECE) {
//                 std::cout << "● "; // Player 2's pieces
//             }
//         }
//         std::cout << std::endl;
//     }
//     std::cout << std::endl;
// }

bool Board::executeMove(int player, int fromRow, int fromCol, int toRow, int toCol) {
    if (!isValidMove(player, fromRow, fromCol, toRow, toCol)) {
        return false;
    }

    board[toRow][toCol] = player;       // Move the piece
    board[fromRow][fromCol] = EMPTY;   // Clear the original position
    std::cout << "Board updated: (" << fromRow << ", " << fromCol << ") -> ("
              << toRow << ", " << toCol << ")\n";
    return true;
}

void Board::captureDirection(int player, int row, int col, int dRow, int dCol) {
    int opponent = (player == P1_PIECE) ? P2_PIECE : P1_PIECE;
    int nextRow = row + dRow;
    int nextCol = col + dCol;

    if (inBounds(nextRow, nextCol) && board[nextRow][nextCol] == opponent) {
        int beyondRow = nextRow + dRow;
        int beyondCol = nextCol + dCol;
        if (!inBounds(beyondRow, beyondCol) || board[beyondRow][beyondCol] == player) {
            board[nextRow][nextCol] = EMPTY; // capture piece
        }
    }
}

void Board::checkAndCapture(int player, int row, int col) {
    captureDirection(player, row, col, -1, 0); // up
    captureDirection(player, row, col, 1, 0);  // down
    captureDirection(player, row, col, 0, -1); // left
    captureDirection(player, row, col, 0, 1);  // right
}

Board::GameResult Board::checkGameEnd() const {
    int p1Count = countPieces(P1_PIECE);
    int p2Count = countPieces(P2_PIECE);

    if (p1Count == 0 && p2Count == 0) return GameResult::Draw;
    if (p1Count == 0) return GameResult::Player2Wins;
    if (p2Count == 0) return GameResult::Player1Wins;

    return GameResult::Ongoing;
}

int Board::getPiece(int row, int col) const {
    if (inBounds(row, col))
        return board[row][col];

    return EMPTY; // return EMPTY for out-of-bounds safeguard
}

int Board::countPieces(int player) const {
    int count = 0;

    for (const auto& row : board)
        for (int cell : row)
            if (cell == player)
                count++;

    return count;
}

bool Board::inBounds(int row, int col) const {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

bool Board::isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol) const {
    // ensure the move starts from a valid position and moves to an empty space
    if (!inBounds(fromRow, fromCol) || !inBounds(toRow, toCol)) return false;
    if (board[fromRow][fromCol] != player) return false;
    if (board[toRow][toCol] != EMPTY) return false; 

    // restrict to horizontal/vertical adjacent moves
    int rowDiff = std::abs(fromRow - toRow);
    int colDiff = std::abs(fromCol - toCol);
    return (rowDiff == 1 && colDiff == 0) || (rowDiff == 0 && colDiff == 1);
}