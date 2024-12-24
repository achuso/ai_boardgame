#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <array>
#include <cassert>
#include <cmath>
#include "GameConstants.h"

class Board {
private:
    std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> board{};

    // Checks if the piece at (row, col) is sandwiched by blocking ends vertically or horizontally
    void checkSelfCapture(int row, int col);

    // Checks and processes captures in a specific direction
    void captureDirection(int row, int col, int dRow, int dCol);

public:
    Board();

    constexpr Board(const Board& other) = default;
    constexpr Board& operator=(const Board& other) = default;

    static bool inBounds(int row, int col);
    bool isBlocking(int row, int col) const; // Helper to check if a cell is blocking

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