#include "TurnManager.h"

TurnManager::TurnManager(Board& gameBoard) 
    : board(gameBoard), currentPlayer(P2_PIECE), movesThisTurn(0) {}

int TurnManager::getCurrentPlayer() const {
    return currentPlayer;
}

void TurnManager::endTurn() {
    movesThisTurn = 0; // reset move count
    currentPlayer = (currentPlayer == P1_PIECE) ? P2_PIECE : P1_PIECE; // switch player
}

bool TurnManager::isMoveLimitExceeded() const {
    int playerPieces = board.countPieces(currentPlayer);
    return (playerPieces == 1 && movesThisTurn >= 1) || (playerPieces > 1 && movesThisTurn >= 2);
}

Board::GameResult TurnManager::getGameState() const {
    return board.checkGameEnd();
}

TurnManager::MoveResult TurnManager::makeMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (isMoveLimitExceeded())
        return MoveResult::MoveLimitExceeded;

    if (board.executeMove(currentPlayer, fromRow, fromCol, toRow, toCol)) {
        board.checkAndCapture(currentPlayer, toRow, toCol);
        movesThisTurn++;
        return MoveResult::Success;
    }

    return MoveResult::InvalidMove;
}