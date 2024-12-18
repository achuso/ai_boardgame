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
    // check standard conditions first
    Board::GameResult result = board.checkGameEnd();
    if (result != Board::GameResult::Ongoing) {
        return result;
    }

    // check the 50-move rule
    if (totalMoves >= 50) {
        int p1Count = board.countPieces(P1_PIECE);
        int p2Count = board.countPieces(P2_PIECE);

        if (p1Count == p2Count) {
            return Board::GameResult::Draw;
        } 
        else if (p1Count > p2Count) {
            return Board::GameResult::Player1Wins;
        } 
        else {
            return Board::GameResult::Player2Wins;
        }
    }

    // if none of the conditions trigger, game continues
    return Board::GameResult::Ongoing;
}

TurnManager::MoveResult TurnManager::makeMove(int fromRow, int fromCol, int toRow, int toCol) {
    // check if we exceeded the move limit
    if (isMoveLimitExceeded()) {
        return MoveResult::MoveLimitExceeded;
    }

    // check if the piece at the target cell has already moved
    if (movedPieces.count({fromRow, fromCol}) || movedPieces.count({toRow, toCol})) {
        return MoveResult::InvalidMove;
    }

    if (board.executeMove(currentPlayer, fromRow, fromCol, toRow, toCol)) {
        movesThisTurn++;
        totalMoves++;
        movedPieces.insert({toRow, toCol});
        board.checkAndCapture(currentPlayer, toRow, toCol);

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