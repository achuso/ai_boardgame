#include "Board.h"

Board::Board() {
    // init board
    for (auto& row : board) row.fill(EMPTY);

    // init player1's pieces (AI)
    board[0][0] = P1_PIECE;
    board[2][0] = P1_PIECE;
    board[4][6] = P1_PIECE;
    board[6][6] = P1_PIECE;

    // init player 2's pieces (human)
    board[0][6] = P2_PIECE;
    board[2][6] = P2_PIECE;
    board[4][0] = P2_PIECE;
    board[6][0] = P2_PIECE;
}

bool Board::isInBounds(int row, int col) {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

bool Board::isOpponentPiece(int row, int col, int currentPiece) const {
    int piece = getPiece(row, col);
    return piece != EMPTY && piece != currentPiece;
}

bool Board::isCellBlocking(int row, int col) const {
    return !isInBounds(row, col) || getPiece(row, col) != EMPTY;
}

void Board::checkAndCapture(int row, int col) {
    // handle captures in four directions
    captureInDirection(row, col, -1, 0); // Up
    captureInDirection(row, col, 1, 0);  // Down
    captureInDirection(row, col, 0, -1); // Left
    captureInDirection(row, col, 0, 1);  // Right

    // check if the moved piece itself is captured
    checkSelfCapture(row, col);
}

void Board::captureInDirection(int row, int col, int dRow, int dCol) {
    int currentPiece = getPiece(row, col);
    int nextRow = row + dRow;
    int nextCol = col + dCol;

    std::vector<std::pair<int, int>> toCapture; // track pieces to capture

    while (isInBounds(nextRow, nextCol)) {
        int piece = getPiece(nextRow, nextCol);

        if (piece == EMPTY) {
            // open space, capture invalid
            return;
        }
        if (piece == currentPiece) {
            // found another piece of the current player -> validate capture
            for (const auto& coord : toCapture) {
                board[coord.first][coord.second] = EMPTY;
            }
            return;
        }
        // found an opponent piece -> mark for potential capture
        toCapture.emplace_back(nextRow, nextCol);

        nextRow += dRow;
        nextCol += dCol;
    }

    // if the edge of the board (wall) is reached, validate capture
    if (!isInBounds(nextRow, nextCol)) {
        for (const auto& coord : toCapture) {
            board[coord.first][coord.second] = EMPTY;
        }
    }
}

void Board::captureSurroundingPieces(int row, int col) {
    // check and capture pieces in all directions
    captureInDirection(row, col, -1, 0); // up
    captureInDirection(row, col, 1, 0);  // down
    captureInDirection(row, col, 0, -1); // left
    captureInDirection(row, col, 0, 1);  // right
}

void Board::checkSelfCapture(int row, int col) {
    int currentPiece = getPiece(row, col);
    if (currentPiece == EMPTY) return;

    // check for vertical sandwich
    bool verticalSandwich =
        (isOpponentPiece(row - 1, col, currentPiece) || !isInBounds(row - 1, col)) &&
        (isOpponentPiece(row + 1, col, currentPiece) || !isInBounds(row + 1, col));

    // check for horizontal sandwich
    bool horizontalSandwich =
        (isOpponentPiece(row, col - 1, currentPiece) || !isInBounds(row, col - 1)) &&
        (isOpponentPiece(row, col + 1, currentPiece) || !isInBounds(row, col + 1));

    // remove the piece if sandwiched
    if (verticalSandwich || horizontalSandwich) {
        board[row][col] = EMPTY;
    }
}

bool Board::isOpponentOrWall(int row, int col, int currentPiece) const {
    return (!isInBounds(row, col)) || isOpponentPiece(row, col, currentPiece);
}

// public methods
bool Board::executeMove(int player, int fromRow, int fromCol, int toRow, int toCol) {
    if (!isValidMove(player, fromRow, fromCol, toRow, toCol)) {
        return false;
    }

    board[toRow][toCol] = board[fromRow][fromCol];
    board[fromRow][fromCol] = EMPTY;

    captureSurroundingPieces(toRow, toCol);
    checkSelfCapture(toRow, toCol);

    return true;
}

Board::GameResult Board::checkGameEnd() const {
    int p1Count = countPieces(P1_PIECE);
    int p2Count = countPieces(P2_PIECE);

    if (p1Count == 0 && p2Count == 0) return GameResult::Draw;
    if (p1Count == 0) return GameResult::Player2Wins;
    if (p2Count == 0) return GameResult::Player1Wins;

    return GameResult::Ongoing;
}

bool Board::isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol) const {
    if (!isInBounds(fromRow, fromCol) || !isInBounds(toRow, toCol)) return false;
    if (board[fromRow][fromCol] != player || board[toRow][toCol] != EMPTY) return false;

    int rowDiff = std::abs(fromRow - toRow);
    int colDiff = std::abs(fromCol - toCol);

    return (rowDiff == 1 && colDiff == 0) || (rowDiff == 0 && colDiff == 1);
}

int Board::getPiece(int row, int col) const {
    return isInBounds(row, col) ? board[row][col] : EMPTY;
}

int Board::countPieces(int player) const {
    int count = 0;
    for (const auto& row : board) {
        for (int cell : row) {
            if (cell == player) {
                count++;
            }
        }
    }
    return count;
}

void Board::printBoard() const {
    for (const auto& row : board) {
        for (int cell : row) {
            std::cout << cell << " ";
        }
        std::cout << "\n";
    }
}