#include "GameWindow.h"
#include <QMessageBox>
#include <QVBoxLayout>

GameWindow::GameWindow(QWidget* parent) : QMainWindow(parent), ai(3) {
    // Initialize the main widget and layout
    QWidget* centralWidget = new QWidget(this);
    gridLayout = new QGridLayout(centralWidget);
    centralWidget->setLayout(gridLayout);
    setCentralWidget(centralWidget);

    // Create the buttons for the grid
    for (int row = 0; row < 7; ++row) {
        for (int col = 0; col < 7; ++col) {
            buttons[row][col] = new QPushButton(this);
            buttons[row][col]->setFixedSize(50, 50);
            gridLayout->addWidget(buttons[row][col], row, col);

            // Connect button clicks to handleCellClick
            connect(buttons[row][col], &QPushButton::clicked, [this, row, col]() {
                handleCellClick(row, col);
            });
        }
    }

    // Initial board rendering
    updateBoard();
}

void GameWindow::updateBoard() {
    for (int row = 0; row < 7; ++row) {
        for (int col = 0; col < 7; ++col) {
            int piece = board.getPiece(row, col);
            if (piece == EMPTY) {
                buttons[row][col]->setText("");
            } else if (piece == P1_PIECE) {
                buttons[row][col]->setText("▲");
            } else if (piece == P2_PIECE) {
                buttons[row][col]->setText("●");
            }
        }
    }
}

void GameWindow::handleCellClick(int row, int col) {
    static int fromRow = -1, fromCol = -1;

    // If a starting position is selected
    if (fromRow == -1 && fromCol == -1) {
        if (board.getPiece(row, col) == currentPlayer) {
            fromRow = row;
            fromCol = col;
        }
    } else {
        // Try to make a move
        if (board.movePiece(currentPlayer, fromRow, fromCol, row, col)) {
            updateBoard();
            fromRow = fromCol = -1;

            // Check for end conditions
            if (board.checkGameEnd()) {
                QMessageBox::information(this, "Game Over", "Game has ended!");
                close();
                return;
            }

            // AI turn if human finished their moves
            if (currentPlayer == P2_PIECE) {
                currentPlayer = P1_PIECE;
                auto bestMove = ai.findBestMove(board);
                board.movePiece(P1_PIECE, bestMove.fromRow, bestMove.fromCol, bestMove.toRow, bestMove.toCol);
                updateBoard();

                // Check game end after AI move
                if (board.checkGameEnd()) {
                    QMessageBox::information(this, "Game Over", "Game has ended!");
                    close();
                    return;
                }

                currentPlayer = P2_PIECE; // Back to human
            }
        } else {
            QMessageBox::warning(this, "Invalid Move", "Try a valid move!");
            fromRow = fromCol = -1;
        }
    }
}
