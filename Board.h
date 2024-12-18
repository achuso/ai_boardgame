#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <array>
#include <cassert>
#include "GameConstants.h"

class Board {
private:
    std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> board;

    bool inBounds(int row, int col) const;
    void captureDirection(int player, int row, int col, int dRow, int dCol);

public:
    Board();

    bool executeMove(int player, int fromRow, int fromCol, int toRow, int toCol);
    void checkAndCapture(int player, int row, int col);
    enum class GameResult { Ongoing, Draw, Player1Wins, Player2Wins };
    GameResult checkGameEnd() const;
    int getPiece(int row, int col) const;
    int countPieces(int player) const;
    bool isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol) const;
};

#endif