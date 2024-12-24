#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

constexpr int EMPTY = 0;
constexpr int P1_PIECE = 1; // ▲ - AI Agent
constexpr int P2_PIECE = 2; // ● - Human Player
constexpr int BOARD_SIZE = 7;
constexpr int MAX_MOVE_RULE = 100; // 50 TURNS, adjust if necessary

struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
};

#endif