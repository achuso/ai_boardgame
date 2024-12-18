#include <iostream>
#include <array>
#include <set>

const int EMPTY = 0;
const int P1_PIECE = 1; // ▲
const int P2_PIECE = 2; // ●

class Board {
private:
    std::array<std::array<int, 7>, 7> board;
    std::set<std::pair<int, int>> movedPieces; // tracks pieces that have moved this turn

    // check if within bounds
    bool inBounds(int row, int col) const {
        return row >= 0 && row < 7 && col >= 0 && col < 7;
    }

public:
    Board() {
        // init board
        for (auto& row : board) row.fill(EMPTY);

        // init p1 pieces
        board[0][0] = P1_PIECE; 
        board[2][0] = P1_PIECE;
        board[4][6] = P1_PIECE; 
        board[6][6] = P1_PIECE;

        // init p2 pieces
        board[0][6] = P2_PIECE; 
        board[2][6] = P2_PIECE;
        board[4][0] = P2_PIECE; 
        board[6][0] = P2_PIECE;
    }

    void printBoard() const {
        for (const auto& row : board) {
            for (int cell : row) {
                if (cell == EMPTY) std::cout << ". ";
                else if (cell == P1_PIECE) std::cout << "▲ ";
                else if (cell == P2_PIECE) std::cout << "● ";
            }
            std::cout << "\n";
        }
    }

    // one move per piece
    bool movePiece(int player, int fromRow, int fromCol, int toRow, int toCol) {
        // check if this piece has already moved
        if (movedPieces.count({fromRow, fromCol})) {
            std::cout << "This piece has already moved this turn\n";
            return false;
        }

        if (isValidMove(player, fromRow, fromCol, toRow, toCol)) {
            board[toRow][toCol] = board[fromRow][fromCol];
            board[fromRow][fromCol] = EMPTY;

            // track the moved piece
            movedPieces.insert({fromRow, fromCol});

            // check for captures
            checkAndCapture(player, toRow, toCol);
            return true;
        }

        return false;
    }

    void resetTurn() {
        movedPieces.clear(); // clear tracked moves at the end of each turn
    }

    bool isValidMove(int player, int fromRow, int fromCol, int toRow, int toCol) {
        if (!inBounds(fromRow, fromCol) || !inBounds(toRow, toCol)) return false;
        if (board[toRow][toCol] != EMPTY) return false;
        if (fromRow != toRow && fromCol != toCol) return false;
        if (board[fromRow][fromCol] != player) return false;

        return true;
    }

    void checkAndCapture(int player, int row, int col) {
        int opponent = (player == P1_PIECE) ? P2_PIECE : P1_PIECE;

        // capture left (between wall and opponent)
        if (col - 1 >= 0 && board[row][col - 1] == opponent && col - 2 < 0)
            board[row][col - 1] = EMPTY;

        // capture right (between opponent and wall)
        if (col + 1 < 7 && board[row][col + 1] == opponent && col + 2 >= 7)
            board[row][col + 1] = EMPTY;

        // surround capture left and right
        if (col - 1 >= 0 && col + 1 < 7 && board[row][col - 1] == opponent 
            && board[row][col + 1] == opponent)
            board[row][col] = EMPTY;

        // capture up (between wall and opponent)
        if (row - 1 >= 0 && board[row - 1][col] == opponent && row - 2 < 0)
            board[row - 1][col] = EMPTY;

        // dapture down (between opponent and wall)
        if (row + 1 < 7 && board[row + 1][col] == opponent && row + 2 >= 7)
            board[row + 1][col] = EMPTY;

        // surround capture up and down
        if (row - 1 >= 0 && row + 1 < 7 && board[row - 1][col] == opponent 
            && board[row + 1][col] == opponent)
            board[row][col] = EMPTY;

        // both players' pieces surrounded (rare case)
        if (row - 1 >= 0 && row + 1 < 7 && board[row - 1][col] != EMPTY 
            && board[row + 1][col] != EMPTY && board[row - 1][col] != board[row + 1][col]) {
            board[row - 1][col] = EMPTY;
            board[row + 1][col] = EMPTY;
        }
    }

    bool checkGameEnd() {
        int p1Count = 0, p2Count = 0;

        for (const auto& row : board)
            for (int cell : row) {
                if (cell == P1_PIECE) p1Count++;
                else if (cell == P2_PIECE) p2Count++;
            }

        // endgame conditions
        if (p1Count == 0 && p2Count == 0) {
            std::cout << "It's a Draw!\n";
            return true;
        } 
        else if (p1Count == 0) {
            std::cout << "Player 2 Wins!\n";
            return true;
        } 
        else if (p2Count == 0) {
            std::cout << "Player 1 Wins!\n";
            return true;
        }

        return false;
    }
};

int main() {
    Board board = Board();
    board.printBoard();
}