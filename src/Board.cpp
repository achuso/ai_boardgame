#include "Board.h"
#include <iostream>
#include <vector>
#include <tuple>

Board::Board() {
    // init board
    for (auto& row : board) {
        row.fill(EMPTY);
    }

    // init player 1's pieces (ai)
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

void Board::printBoard() const {
    std::cout << "Board State:\n";
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int piece = board[row][col];
            if (piece == EMPTY) {
                std::cout << ". ";
            } 
            else if (piece == P1_PIECE) {
                std::cout << "X ";
            } 
            else if (piece == P2_PIECE) {
                std::cout << "O ";
            }
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

bool Board::inBounds(int row, int col) const {
    return (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE);
}

bool Board::isBlocking(int row, int col) const {
    if (!inBounds(row, col)) return true; // out of bounds
    int piece = board[row][col];
    return (piece == P1_PIECE || piece == P2_PIECE);
}

int Board::getPiece(int row, int col) const {
    if (!inBounds(row, col)) return EMPTY;
    return board[row][col];
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

Board::GameResult Board::checkGameEnd() const {
    int p1Count = countPieces(P1_PIECE);
    int p2Count = countPieces(P2_PIECE);

    if (p1Count == 0 && p2Count == 0) 
        return GameResult::Draw;
    if (p1Count == 0) 
        return GameResult::Player2Wins;
    if (p2Count == 0) 
        return GameResult::Player1Wins;

    return GameResult::Ongoing;
}

bool Board::isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol) const {
    if (!inBounds(fromRow, fromCol) || !inBounds(toRow, toCol)) return false;
    if (board[fromRow][fromCol] != player) return false;
    if (board[toRow][toCol] != EMPTY) return false; 

    int rowDiff = std::abs(fromRow - toRow);
    int colDiff = std::abs(fromCol - toCol);
    // only allow orthogonal adjacency
    return (rowDiff == 1 && colDiff == 0) || (rowDiff == 0 && colDiff == 1);
}

bool Board::executeMove(int player, int fromRow, int fromCol, int toRow, int toCol) {
    if (!isValidMove(player, fromRow, fromCol, toRow, toCol)) {
        return false;
    }
    // move the piece
    board[toRow][toCol] = player;
    board[fromRow][fromCol] = EMPTY;

    // check captures triggered by the new position
    checkAndCapture(player, toRow, toCol);
    return true;
}

void Board::captureRow(int r) {
    if (r < 0 || r >= BOARD_SIZE) return;

    std::vector<std::tuple<int, int, int>> segments; // (startCol, endCol, pieceType)

    int c = 0;
    while (c < BOARD_SIZE) {
        int current = getPiece(r, c);
        if (current == EMPTY) {
            ++c;
            continue;
        }
        // find start of a segment of the same piece type
        int pieceType = current;
        int startCol = c;
        while (c < BOARD_SIZE && getPiece(r, c) == pieceType) {
            ++c;
        }
        int endCol = c - 1; // c now is the first non-piece or out-of-bounds
        segments.emplace_back(startCol, endCol, pieceType);
    }

    // mark captured cells in a list, then remove them after
    std::vector<std::pair<int, int>> toClear;

    for (const auto& seg : segments) {
        int startCol, endCol, segType;
        std::tie(startCol, endCol, segType) = seg;

        // bounding columns
        int leftBoundCol  = startCol - 1;
        int rightBoundCol = endCol + 1;

        auto getBoundType = [&](int cc) -> int {
            if (!inBounds(r, cc)) {
                return -1; // treat out-of-bounds as wall
            }
            return getPiece(r, cc);
        };

        int leftType  = getBoundType(leftBoundCol);
        int rightType = getBoundType(rightBoundCol);

        // conditions: 
        // 1. if top and bottom bounding "types" are the same and not segType or EMPTY
        // 2. if one side is wall (-1) and the other side is an enemy piece

        bool sameSandwich = (leftType != EMPTY && leftType == rightType && leftType != segType);
        bool wallAndEnemy = false;
        if (leftType == -1 && rightType != EMPTY && rightType != segType && rightType != -1) {
            wallAndEnemy = true;
        }
        if (rightType == -1 && leftType != EMPTY && leftType != segType && leftType != -1) {
            wallAndEnemy = true;
        }

        if (sameSandwich || wallAndEnemy) {
            for (int colToRemove = startCol; colToRemove <= endCol; ++colToRemove) {
                toClear.emplace_back(r, colToRemove);
            }
        }
    }

    // remove them
    for (const auto& cell : toClear) {
        int rr = cell.first;
        int cc = cell.second;
        board[rr][cc] = EMPTY;
    }
}

void Board::captureCol(int c) {
    if (c < 0 || c >= BOARD_SIZE) 
        return;

    std::vector<std::tuple<int, int, int>> segments;

    int r = 0;
    while (r < BOARD_SIZE) {
        int current = getPiece(r, c);
        if (current == EMPTY) {
            ++r;
            continue;
        }
        // start of a segment
        int pieceType = current;
        int startRow = r;
        while (r < BOARD_SIZE && getPiece(r, c) == pieceType) {
            ++r;
        }
        int endRow = r - 1; 
        segments.emplace_back(startRow, endRow, pieceType);
    }

    std::vector<std::pair<int, int>> toClear;

    for (const auto& seg : segments) {
        int startRow, endRow, segType;
        std::tie(startRow, endRow, segType) = seg;

        // bounding rows
        int upBound    = startRow - 1;
        int downBound  = endRow + 1;

        auto getBoundType = [&](int rr) -> int {
            if (!inBounds(rr, c)) {
                return -1; // treat out-of-bounds as "wall"
            }
            return getPiece(rr, c);
        };

        int topType    = getBoundType(upBound);
        int bottomType = getBoundType(downBound);


        // same capture conditions as in captureRow
        bool sameSandwich = (topType != EMPTY && topType == bottomType && topType != segType);
        bool wallAndEnemy = false;
        if (topType == -1 && bottomType != EMPTY && bottomType != segType && bottomType != -1) {
            wallAndEnemy = true;
        }
        if (bottomType == -1 && topType != EMPTY && topType != segType && topType != -1) {
            wallAndEnemy = true;
        }

        if (sameSandwich || wallAndEnemy) {
            // mark entire segment for removal
            for (int rowToRemove = startRow; rowToRemove <= endRow; ++rowToRemove) {
                toClear.emplace_back(rowToRemove, c);
            }
        }
    }

    // remove them
    for (const auto& cell : toClear) {
        int rr = cell.first;
        int cc = cell.second;
        board[rr][cc] = EMPTY;
    }
}

void Board::checkAndCapture(/*int player,*/ int row, int col) {
    bool changed;
    do {
        changed = false;
        // capture row
        int beforeClearRow = countPieces(P1_PIECE) + countPieces(P2_PIECE);
        captureRow(row);
        int afterClearRow = countPieces(P1_PIECE) + countPieces(P2_PIECE);
        if (afterClearRow < beforeClearRow) {
            changed = true;
        }

        // capture column
        int beforeClearCol = afterClearRow;
        captureCol(col);
        int afterClearCol = countPieces(P1_PIECE) + countPieces(P2_PIECE);
        if (afterClearCol < afterClearRow) {
            changed = true;
        }

    } while (changed);
}