#include <iostream>
#include "Board.h"

Board::Board() {
    // Initialize the board with empty cells
    for (auto& row : board) row.fill(EMPTY);

    // Initialize Player 1's pieces
    board[0][0] = P1_PIECE;
    board[2][0] = P1_PIECE;
    board[4][6] = P1_PIECE;
    board[6][6] = P1_PIECE;

    // Initialize Player 2's pieces
    board[0][6] = P2_PIECE;
    board[2][6] = P2_PIECE;
    board[4][0] = P2_PIECE;
    board[6][0] = P2_PIECE;
}

bool Board::inBounds(int row, int col) {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

bool Board::isBlocking(int row, int col) const {
    // A cell is blocking if it's out-of-bounds (wall) or contains a piece (P1 or P2)
    if (!inBounds(row, col)) return true;
    int piece = board[row][col];
    return (piece == P1_PIECE || piece == P2_PIECE);
}

int Board::getPiece(int row, int col) const {
    if (inBounds(row, col))
        return board[row][col];

    return EMPTY; // Return EMPTY for out-of-bounds safeguard
}

int Board::countPieces(int player) const {
    int count = 0;
    for (const auto& r : board)
        for (int cell : r)
            if (cell == player)
                count++;
    return count;
}

bool Board::isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol) const {
    // Ensure the move starts from a valid position and moves to an empty space
    if (!inBounds(fromRow, fromCol) || !inBounds(toRow, toCol)) return false;
    if (board[fromRow][fromCol] != player) return false;
    if (board[toRow][toCol] != EMPTY) return false;

    // Restrict to horizontal/vertical adjacent moves
    int rowDiff = std::abs(fromRow - toRow);
    int colDiff = std::abs(fromCol - toCol);
    return (rowDiff == 1 && colDiff == 0) || (rowDiff == 0 && colDiff == 1);
}

Board::GameResult Board::checkGameEnd() const {
    int p1Count = countPieces(P1_PIECE);
    int p2Count = countPieces(P2_PIECE);

    if (p1Count == 0 && p2Count == 0) return GameResult::Draw;
    if (p1Count == 0) return GameResult::Player2Wins;
    if (p2Count == 0) return GameResult::Player1Wins;

    return GameResult::Ongoing;
}

bool Board::executeMove(int player, int fromRow, int fromCol, int toRow, int toCol) {
    if (!isValidMove(player, fromRow, fromCol, toRow, toCol)) {
        return false;
    }

    board[toRow][toCol] = board[fromRow][fromCol];
    board[fromRow][fromCol] = EMPTY;

    return true;
}

void Board::captureDirection(int row, int col, int dRow, int dCol) {
    int currentPiece = board[row][col];
    int nextRow = row + dRow;
    int nextCol = col + dCol;

    // Check the adjacent cell
    if (inBounds(nextRow, nextCol) && board[nextRow][nextCol] != EMPTY &&
        board[nextRow][nextCol] != currentPiece) {
        int beyondRow = nextRow + dRow;
        int beyondCol = nextCol + dCol;

        // Check if the cell beyond is a wall or the same piece type
        if (!inBounds(beyondRow, beyondCol) || board[beyondRow][beyondCol] == currentPiece) {
            board[nextRow][nextCol] = EMPTY; // Capture the middle piece
        }
        }
}

void Board::checkAndCapture(int row, int col) {
    // Check in all four cardinal directions for captures
    captureDirection(row, col, -1, 0); // up
    captureDirection(row, col, 1, 0);  // down
    captureDirection(row, col, 0, -1); // left
    captureDirection(row, col, 0, 1);  // right

    // Check if the current piece itself is sandwiched and should be captured
    checkSelfCapture(row, col);
}

void Board::checkSelfCapture(int row, int col) {
    int piece = getPiece(row, col);
    if (piece == EMPTY) return; // No piece to capture

    int opponent = (piece == P1_PIECE) ? P2_PIECE : P1_PIECE;

    // Check vertical sandwich
    int upRow = row - 1;
    int downRow = row + 1;

    bool verticallySandwiched =
        ((isBlocking(upRow, col) && getPiece(upRow, col) == opponent) || !inBounds(upRow, col)) &&
        ((isBlocking(downRow, col) && getPiece(downRow, col) == opponent) || !inBounds(downRow, col));

    // Check horizontal sandwich
    int leftCol = col - 1;
    int rightCol = col + 1;

    bool horizontallySandwiched =
        ((isBlocking(row, leftCol) && getPiece(row, leftCol) == opponent) || !inBounds(row, leftCol)) &&
        ((isBlocking(row, rightCol) && getPiece(row, rightCol) == opponent) || !inBounds(row, rightCol));

    // If sandwiched either vertically or horizontally, remove the piece
    if (verticallySandwiched || horizontallySandwiched) {
        board[row][col] = EMPTY;
    }
}