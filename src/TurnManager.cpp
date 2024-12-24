#include "TurnManager.h"

TurnManager::TurnManager(const Board& gameBoard)
    : board(gameBoard), currentPlayer(P1_PIECE), movesThisTurn(0), totalMoves(0) {}

void TurnManager::startTurn() {
    movesThisTurn = 0;
    movedPieces.clear();
}

void TurnManager::endTurn() {
    currentPlayer = (currentPlayer == P1_PIECE) ? P2_PIECE : P1_PIECE;
    startTurn();
}

bool TurnManager::validateMove(int fromRow, int fromCol, int toRow, int toCol) const {
    return !isMoveLimitExceeded()
        && !movedPieces.count({fromRow, fromCol})
        && board.isValidMove(currentPlayer, fromRow, fromCol, toRow, toCol);
}

void TurnManager::addMoveToHistory(int fromRow, int fromCol, int toRow, int toCol) {
    moveHistory.push_back({fromRow, fromCol, toRow, toCol});
}

TurnManager::MoveResult TurnManager::makeMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (!validateMove(fromRow, fromCol, toRow, toCol)) {
        return MoveResult::InvalidMove;
    }

    if (board.executeMove(currentPlayer, fromRow, fromCol, toRow, toCol)) {
        movesThisTurn++;
        totalMoves++;
        movedPieces.insert({toRow, toCol});

        // handle all capture scenarios
        board.checkAndCapture(toRow, toCol);

        return MoveResult::Success;
    }

    return MoveResult::InvalidMove;
}

bool TurnManager::isMoveLimitExceeded() const {
    int playerPieces = board.countPieces(currentPlayer);
    return (playerPieces == 1 && movesThisTurn >= 1) || (playerPieces > 1 && movesThisTurn >= 2);
}

bool TurnManager::hasPieceMoved(int row, int col) const {
    return movedPieces.count({row, col});
}

const Board* TurnManager::getBoard() const {
    return &board;
}

Board::GameResult TurnManager::getGameState() const {
    Board::GameResult result = board.checkGameEnd();
    if (result != Board::GameResult::Ongoing) {
        return result;
    }

    if (totalMoves >= MAX_MOVE_RULE) {
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

    return Board::GameResult::Ongoing;
}

int TurnManager::getCurrentPlayer() const {
    return currentPlayer;
}