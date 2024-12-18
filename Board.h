#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <set>
#include "GameConstants.h"

class Board {
private:
    std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> board;
    std::set<std::pair<int, int>> movedPieces;

    bool inBounds(int row, int col) const;
    void captureDirection(int player, int row, int col, int dRow, int dCol);

public:
    Board();

    bool executeMove(int player, int fromRow, int fromCol, int toRow, int toCol);
    void resetTurn();
    void checkAndCapture(int player, int row, int col);
    bool checkGameEnd();
    bool hasExceededMoveLimit() const;
    int getPiece(int row, int col) const;
    int getMovedPiecesSize() const;
    int countPieces(int player) const;
    bool isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol) const;
};

#endif