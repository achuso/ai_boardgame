#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include "Board.h"
#include "GameConstants.h"
#include <set>
#include <vector>

class TurnManager {
public:
    enum class MoveResult { Success, InvalidMove, MoveLimitExceeded };

private:
    Board board;
    int currentPlayer;
    std::set<std::pair<int, int>> movedPieces;
    std::vector<Move> moveHistory;
    int movesThisTurn;
    int totalMoves;

    // helper methods
    [[nodiscard]] bool validateMove(int fromRow, int fromCol, int toRow, int toCol) const;
    void addMoveToHistory(int fromRow, int fromCol, int toRow, int toCol);

public:
    explicit TurnManager(const Board& gameBoard);

    [[nodiscard]] const Board* getBoard() const;
    [[nodiscard]] int getCurrentPlayer() const;
    [[nodiscard]] int getTotalMoves() const { return totalMoves; }
    [[nodiscard]] bool hasPieceMoved(int row, int col) const;

    // turn-related methods
    void startTurn();
    void endTurn();

    // move-related methods
    MoveResult makeMove(int fromRow, int fromCol, int toRow, int toCol);
    [[nodiscard]] bool isMoveLimitExceeded() const;

    // game state
    [[nodiscard]] Board::GameResult getGameState() const;
};

#endif