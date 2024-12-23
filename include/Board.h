#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <array>
#include <cassert>
#include <cmath>
#include "GameConstants.h"

class Board {
private:
    std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> board;

    void captureRow(int r);
    void captureCol(int c);

public:
    Board();

    // copy/assignment constructors
    constexpr Board(const Board& other) = default;
    constexpr Board& operator=(const Board& other) = default;

    bool inBounds(int row, int col) const;
    bool isBlocking(int row, int col) const; // helper to check if cell is blocking

    void printBoard() const; 
    bool executeMove(int player, int fromRow, int fromCol, int toRow, int toCol);
    void checkAndCapture(int row, int col);

    enum class GameResult { Ongoing, Draw, Player1Wins, Player2Wins };
    GameResult checkGameEnd() const;

    int getPiece(int row, int col) const;
    int countPieces(int player) const;
    bool isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol) const;
};

#endif