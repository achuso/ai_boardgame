#ifndef BOARD_H
#define BOARD_H

#include "GameConstants.h"
#include <array>

class Board {
private:
    std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> board{};

    static bool isInBounds(int row, int col); // Static utility for boundary checks
    bool isOpponentPiece(int row, int col, int currentPiece) const;
    bool isCellBlocking(int row, int col) const;

    // capture Logic
    void captureInDirection(int row, int col, int dRow, int dCol);
    void captureSurroundingPieces(int row, int col);
    void checkSelfCapture(int row, int col);

public:
    enum class GameResult { Ongoing, Draw, Player1Wins, Player2Wins };

    Board();

    void checkAndCapture(int row, int col);

    // board ops
    bool executeMove(int player, int fromRow, int fromCol, int toRow, int toCol);
    void printBoard() const;

    // game state and validation
    GameResult checkGameEnd() const;
    bool isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol) const;

    // board info
    int getPiece(int row, int col) const;
    int countPieces(int player) const;
};

#endif