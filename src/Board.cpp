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

    // Test board configurations: (AI is expected to move into position for capture)

    // 1) XOXOX -> X...X
    // board[1][1] = P1_PIECE;
    // board[1][2] = P2_PIECE;
    // board[2][3] = P1_PIECE;
    // board[1][4] = P2_PIECE;
    // board[1][5] = P1_PIECE;

    // 2) XOOOX -> X...X
    // board[1][1] = P1_PIECE;
    // board[1][2] = P2_PIECE;
    // board[1][3] = P2_PIECE;
    // board[1][4] = P2_PIECE;
    // board[2][5] = P1_PIECE;

    // 3) XOOOOX(wall) -> X.....(wall)
    // board[1][1] = P1_PIECE;
    // board[1][2] = P2_PIECE;
    // board[1][3] = P2_PIECE;
    // board[1][4] = P2_PIECE;
    // board[1][5] = P2_PIECE;
    // board[2][6] = P1_PIECE;
}

bool Board::isInBounds(const int row, const int col) {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

bool Board::isOpponentPiece(const int row, const int col, const int currentPiece) const {
    int piece = getPiece(row, col);
    return piece != EMPTY && piece != currentPiece;
}

bool Board::isCellBlocking(const int row, const int col) const {
    return !isInBounds(row, col) || getPiece(row, col) != EMPTY;
}

void Board::checkAndCapture(int row, int col) {
    std::vector<std::pair<int, int>> toCapture;

    // collect all capturable pieces in four directions
    collectCaptures(row, col, -1, 0, toCapture); // up
    collectCaptures(row, col, 1, 0, toCapture);  // down
    collectCaptures(row, col, 0, -1, toCapture); // left
    collectCaptures(row, col, 0, 1, toCapture);  // right

    // check if the moved piece itself is captured
    if (isSelfCaptured(row, col)) toCapture.emplace_back(row, col);

    // remove all capturable pieces at once
    for (const auto&[fst, snd] : toCapture)
        board[fst][snd] = EMPTY;
}

void Board::collectCaptures(const int row, const int col,
                            const int dRow, const int dCol,
                            std::vector<std::pair<int, int>>& toCapture) const {
    int currentPiece = getPiece(row, col);
    int nextRow = row + dRow;
    int nextCol = col + dCol;

    std::vector<std::pair<int, int>> potentialCaptures;

    while (isInBounds(nextRow, nextCol)) {
        const int piece = getPiece(nextRow, nextCol);

        // open space, capture invalid
        if (piece == EMPTY)
            return;
        // found another piece of the current player -> validate capture
        if (piece == currentPiece) {
            toCapture.insert(toCapture.end(), potentialCaptures.begin(), potentialCaptures.end());
            return;
        }

        // found an opponent piece -> mark for potential capture
        potentialCaptures.emplace_back(nextRow, nextCol);

        nextRow += dRow;
        nextCol += dCol;
    }

    // if the edge of the board (wall) is reached, validate capture
    if (!isInBounds(nextRow, nextCol))
        toCapture.insert(toCapture.end(), potentialCaptures.begin(), potentialCaptures.end());
}

bool Board::isSelfCaptured(const int row, const int col) const {
    const int currentPiece = getPiece(row, col);
    if (currentPiece == EMPTY) return false;

    // check for vertical sandwich
    const bool verticalSandwich =
        (isOpponentPiece(row - 1, col, currentPiece) || !isInBounds(row - 1, col)) &&
        (isOpponentPiece(row + 1, col, currentPiece) || !isInBounds(row + 1, col));

    // check for horizontal sandwich
    const bool horizontalSandwich =
        (isOpponentPiece(row, col - 1, currentPiece) || !isInBounds(row, col - 1)) &&
        (isOpponentPiece(row, col + 1, currentPiece) || !isInBounds(row, col + 1));

    // return true if sandwiched
    return verticalSandwich || horizontalSandwich;
}

bool Board::isOpponentOrWall(const int row, const int col, const int currentPiece) const {
    return (!isInBounds(row, col)) || isOpponentPiece(row, col, currentPiece);
}

bool Board::executeMove(const int player, const int fromRow, const int fromCol,
                        const int toRow, const int toCol) {
    if (!isValidMove(player, fromRow, fromCol, toRow, toCol)) {
        return false;
    }

    board[toRow][toCol] = board[fromRow][fromCol];
    board[fromRow][fromCol] = EMPTY;

    checkAndCapture(toRow, toCol);

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

bool Board::isValidMove(const int player, const int fromRow, const int fromCol,
                        const int toRow, const int toCol) const {
    if (!isInBounds(fromRow, fromCol) || !isInBounds(toRow, toCol)) return false;
    if (board[fromRow][fromCol] != player || board[toRow][toCol] != EMPTY) return false;

    int rowDiff = std::abs(fromRow - toRow);
    int colDiff = std::abs(fromCol - toCol);

    return (rowDiff == 1 && colDiff == 0) || (rowDiff == 0 && colDiff == 1);
}

int Board::getPiece(const int row, const int col) const {
    return isInBounds(row, col) ? board[row][col] : EMPTY;
}

int Board::countPieces(const int player) const {
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