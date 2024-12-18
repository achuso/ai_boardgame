#include "TurnManager.h"
#include <iostream>

TurnManager::TurnManager(const Board& gameBoard)
    : board(gameBoard), currentPlayer(P1_PIECE), movesThisTurn(0), totalMoves(0) {}

TurnManager::TurnManager(const TurnManager& other)
    : board(other.board),
      currentPlayer(other.currentPlayer),
      movesThisTurn(other.movesThisTurn),
      movedPieces(other.movedPieces),
      totalMoves(other.totalMoves) {}

TurnManager& TurnManager::operator=(const TurnManager& other) {
    if (this != &other) {
        board = other.board;
        currentPlayer = other.currentPlayer;
        movesThisTurn = other.movesThisTurn;
        movedPieces = other.movedPieces;
        totalMoves = other.totalMoves;
    }
    return *this;
}

int TurnManager::getCurrentPlayer() const {
    return currentPlayer;
}

bool TurnManager::isMoveLimitExceeded() const {
    int playerPieces = board.countPieces(currentPlayer);
    return (playerPieces == 1 && movesThisTurn >= 1) || (playerPieces > 1 && movesThisTurn >= 2);
}

Board::GameResult TurnManager::getGameState() const {
    return board.checkGameEnd();
}

TurnManager::MoveResult TurnManager::makeMove(int fromRow, int fromCol, int toRow, int toCol) {
    // Check if we exceeded the move limit
    if (isMoveLimitExceeded()) {
        return MoveResult::MoveLimitExceeded;
    }

    // Check if the piece at the target cell has already moved
    // Now we check if the destination cell has been used, after a piece moves, that piece "lives" at the toRow,toCol.
    if (movedPieces.count({fromRow, fromCol}) || movedPieces.count({toRow, toCol})) {
        // The above condition ensures that if this piece was just moved to (toRow, toCol) previously, 
        // it won't move again from that new spot.
        return MoveResult::InvalidMove;
    }

    if (board.executeMove(currentPlayer, fromRow, fromCol, toRow, toCol)) {
        movesThisTurn++;
        totalMoves++;
        // Insert the new position of the piece into movedPieces.
        movedPieces.insert({toRow, toCol});

        return MoveResult::Success;
    }

    return MoveResult::InvalidMove;
}

void TurnManager::endTurn() {
    movesThisTurn = 0;
    movedPieces.clear();
    currentPlayer = (currentPlayer == P1_PIECE) ? P2_PIECE : P1_PIECE;
}

const Board* TurnManager::getBoard() const {
    return &board;
}
