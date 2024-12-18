#include <iostream>
#include <array>
#include <set>
#include <cassert>

#include "Board.h"

Board::Board() {
    // init board
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

bool Board::executeMove(int player, int fromRow, int fromCol, int toRow, int toCol) {
    if (!isValidMove(player, fromRow, fromCol, toRow, toCol)) {
        return false;
    }
    board[toRow][toCol] = player;
    board[fromRow][fromCol] = EMPTY;
    movedPieces.insert({toRow, toCol});
    return true;
}

void Board::resetTurn() {
    movedPieces.clear();
}

void Board::captureDirection(int player, int row, int col, int dRow, int dCol) {
    int opponent = (player == P1_PIECE) ? P2_PIECE : P1_PIECE;
    int nextRow = row + dRow;
    int nextCol = col + dCol;

    if (inBounds(nextRow, nextCol) && board[nextRow][nextCol] == opponent) {
        int beyondRow = nextRow + dRow;
        int beyondCol = nextCol + dCol;
        if (!inBounds(beyondRow, beyondCol) || board[beyondRow][beyondCol] == player) {
            board[nextRow][nextCol] = EMPTY;
        }
    }
}

void Board::checkAndCapture(int player, int row, int col) {
    captureDirection(player, row, col, -1, 0); // Up
    captureDirection(player, row, col, 1, 0);  // Down
    captureDirection(player, row, col, 0, -1); // Left
    captureDirection(player, row, col, 0, 1);  // Right
}

bool Board::checkGameEnd() {
    int p1Count = countPieces(P1_PIECE);
    int p2Count = countPieces(P2_PIECE);

    if (p1Count == 0 && p2Count == 0) {
        std::cout << "It's a Draw!\n";
        return true;
    }
    if (p1Count == 0) {
        std::cout << "Player 2 Wins!\n";
        return true;
    }
    if (p2Count == 0) {
        std::cout << "Player 1 Wins!\n";
        return true;
    }
    return false;
}

bool Board::hasExceededMoveLimit() const {
    return movedPieces.size() >= 2; // Max two moves per turn
}

int Board::getPiece(int row, int col) const {
    assert(inBounds(row, col) && "Attempted to access out-of-bounds cell");
    return board[row][col];
}

int Board::getMovedPiecesSize() const {
    return movedPieces.size();
}

bool Board::inBounds(int row, int col) const {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

int Board::countPieces(int player) const {
    int count = 0;

    for (const auto& row : board) {
        for (int cell : row) {
            if (cell == player) {
                count++;
            }
        }
    }

    return count;
}

bool Board::isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol) const {
    if (!inBounds(fromRow, fromCol) || !inBounds(toRow, toCol))
        return false;
    if (board[fromRow][fromCol] != player)
        return false;
    if (board[toRow][toCol] != EMPTY) 
        return false;
    if (fromRow != toRow && fromCol != toCol)
        return false;
    if (std::abs(fromRow - toRow) > 1 || std::abs(fromCol - toCol) > 1)
        return false;
    return true;
}
