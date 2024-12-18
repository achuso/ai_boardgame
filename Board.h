#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <set>
#include "GameConstants.h"


class Board {
private:
    std::array<std::array<int, 7>, 7> board;
    std::set<std::pair<int, int>> movedPieces; // tracks pieces that have moved this turn

    bool inBounds(int row, int col) const;

public:
    Board();

    void printBoard() const;
    bool movePiece(int player, int fromRow, int fromCol, int toRow, int toCol);
    void resetTurn();
    bool isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol);
    void checkAndCapture(int player, int row, int col);
    bool checkGameEnd();
    int getPiece(int row, int col) const { return board[row][col]; }
    const std::array<std::array<int, 7>, 7>& getBoard() const { return board; }
};

#endif
