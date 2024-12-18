#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include <set>
#include "Board.h"

class TurnManager {
public:
    enum class MoveResult { Success, InvalidMove, MoveLimitExceeded };

private:
    Board board;  
    int currentPlayer; 
    std::set<std::pair<int, int>> movedPieces; 
    int movesThisTurn; 
    int totalMoves = 0; 

public:
    TurnManager(const Board& gameBoard);
    TurnManager(const TurnManager& other);
    TurnManager& operator=(const TurnManager& other);

    const Board* getBoard() const;
    int getCurrentPlayer() const;
    void endTurn();
    MoveResult makeMove(int fromRow, int fromCol, int toRow, int toCol);
    bool isMoveLimitExceeded() const;
    Board::GameResult getGameState() const;

    int getTotalMoves() const { return totalMoves; }
};

#endif