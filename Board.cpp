#include <iostream>
#include <array>
#include <set>

#include "Board.h"

// init board
Board::Board() {
    for (auto& row : board) row.fill(EMPTY);

    // player 1 pieces
    board[0][0] = P1_PIECE; 
    board[2][0] = P1_PIECE;
    board[4][6] = P1_PIECE; 
    board[6][6] = P1_PIECE;

    // player 2 pieces
    board[0][6] = P2_PIECE; 
    board[2][6] = P2_PIECE;
    board[4][0] = P2_PIECE; 
    board[6][0] = P2_PIECE;
}

void Board::printBoard() const {
    for (const auto& row : board) {
        for (int cell : row) {
            if (cell == EMPTY) std::cout << ". ";
            else if (cell == P1_PIECE) std::cout << "▲ ";
            else if (cell == P2_PIECE) std::cout << "● ";
        }
        std::cout << "\n";
    }
}

// move piece and check for validity/captures
bool Board::movePiece(int player, int fromRow, int fromCol, int toRow, int toCol) {
    // check if the piece has already been moved
    if (movedPieces.count({fromRow, fromCol})) {
        std::cout << "This piece has already moved this turn.\n";
        return false;
    }

    if (isValidMove(player, fromRow, fromCol, toRow, toCol)) {
        board[toRow][toCol] = board[fromRow][fromCol];
        board[fromRow][fromCol] = EMPTY;

        // track the moved piece
        movedPieces.insert({fromRow, fromCol});

        // check for captures
        checkAndCapture(player, toRow, toCol);
        return true;
    }

    return false;
}

// reset pieces that have moved at the end of a turn
void Board::resetTurn() {
    movedPieces.clear();
}

bool Board::isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol) const {
    if (!inBounds(fromRow, fromCol) || !inBounds(toRow, toCol)) return false;
    if (board[toRow][toCol] != EMPTY) return false;
    if (fromRow != toRow && fromCol != toCol) return false;
    if (board[fromRow][fromCol] != player) return false;

    return true;
}

void Board::checkAndCapture(int player, int row, int col) {
    int opponent = (player == P1_PIECE) ? P2_PIECE : P1_PIECE;

    // capture left (between wall and opponent)
    if (col - 1 >= 0 && board[row][col - 1] == opponent && col - 2 < 0)
        board[row][col - 1] = EMPTY;

    // capture right (between opponent and wall)
    if (col + 1 < 7 && board[row][col + 1] == opponent && col + 2 >= 7)
        board[row][col + 1] = EMPTY;

    // surround capture left and right
    if (col - 1 >= 0 && col + 1 < 7 && board[row][col - 1] == opponent 
        && board[row][col + 1] == opponent)
        board[row][col] = EMPTY;

    // capture up (between wall and opponent)
    if (row - 1 >= 0 && board[row - 1][col] == opponent && row - 2 < 0)
        board[row - 1][col] = EMPTY;

    // capture down (between opponent and wall)
    if (row + 1 < 7 && board[row + 1][col] == opponent && row + 2 >= 7)
        board[row + 1][col] = EMPTY;

    // surround capture up and down
    if (row - 1 >= 0 && row + 1 < 7 && board[row - 1][col] == opponent 
        && board[row + 1][col] == opponent)
        board[row][col] = EMPTY;

    // mutual capture (rare occurrance)
    if (row - 1 >= 0 && row + 1 < 7 && board[row - 1][col] != EMPTY 
        && board[row + 1][col] != EMPTY && board[row - 1][col] != board[row + 1][col]) {
        board[row - 1][col] = EMPTY;
        board[row + 1][col] = EMPTY;
    }
}

bool Board::checkGameEnd() {
    int p1Count = 0, p2Count = 0;

    for (const auto& row : board) {
        for (int cell : row) {
            if (cell == P1_PIECE) p1Count++;
            else if (cell == P2_PIECE) p2Count++;
        }
    }
    // Endgame conditions
    if (p1Count == 0 && p2Count == 0) {
        std::cout << "It's a Draw!\n";
        return true;
    } 
    else if (p1Count == 0) {
        std::cout << "Player 2 Wins!\n";
        return true;
    } 
    else if (p2Count == 0) {
        std::cout << "Player 1 Wins!\n";
        return true;
    }

    return false;
}

// Check if a cell is within bounds
bool Board::inBounds(int row, int col) const {
    return row >= 0 && row < 7 && col >= 0 && col < 7;
}
