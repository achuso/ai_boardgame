#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include "Board.h"

class TurnManager {
public:
    enum class MoveResult { Success, InvalidMove, MoveLimitExceeded };

private:
    Board& board;
    int currentPlayer; // tracks current player
    int movesThisTurn; // track the number of moves made this turn

public:
    TurnManager(Board& gameBoard);

    const Board& getBoard() const;
    int getCurrentPlayer() const;
    void endTurn();
    MoveResult makeMove(int fromRow, int fromCol, int toRow, int toCol);
    bool isMoveLimitExceeded() const;
    Board::GameResult getGameState() const;
};

#endif