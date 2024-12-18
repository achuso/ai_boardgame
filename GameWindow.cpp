#include "GameWindow.h"
#include <QMessageBox>
#include <iostream>

GameWindow::GameWindow(QWidget* parent, int aiDepth)
    : QMainWindow(parent), board(), turnManager(board), ai(aiDepth) {
    QWidget* centralWidget = new QWidget(this);
    gridLayout = new QGridLayout(centralWidget);
    centralWidget->setLayout(gridLayout);
    setCentralWidget(centralWidget);

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            buttons[row][col] = new QPushButton(this);
            buttons[row][col]->setFixedSize(50, 50);
            gridLayout->addWidget(buttons[row][col], row, col);

            connect(buttons[row][col], &QPushButton::clicked, [this, row, col]() {
                handleCellClick(row, col);
            });
        }
    }

    updateBoard();
    startTurn();
}

void GameWindow::updateBoard() {
    const Board* b = turnManager.getBoard();
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int piece = b->getPiece(row, col);
            buttons[row][col]->setStyleSheet(""); // Reset style
            if (piece == EMPTY) {
                buttons[row][col]->setText("");
            } 
            else if (piece == P1_PIECE) {
                buttons[row][col]->setText("▲");
            } 
            else if (piece == P2_PIECE) {
                buttons[row][col]->setText("●");
            }
        }
    }
}

void GameWindow::handleCellClick(int row, int col) {
    static int fromRow = -1, fromCol = -1;
    const Board* b = turnManager.getBoard();

    if (fromRow == -1 && fromCol == -1) {
        // select the piece
        if (b->getPiece(row, col) == turnManager.getCurrentPlayer()) {
            fromRow = row;
            fromCol = col;
            buttons[row][col]->setStyleSheet("background-color: blue;");
            // enable entire board for selecting a destination
            for (int r = 0; r < BOARD_SIZE; ++r) {
                for (int c = 0; c < BOARD_SIZE; ++c) {
                    buttons[r][c]->setEnabled(true);
                }
            }
        } else {
            QMessageBox::warning(this, "Invalid Selection", "Please select your own piece!");
        }
    } else {
        // attempt the move
        auto moveResult = turnManager.makeMove(fromRow, fromCol, row, col);
        buttons[fromRow][fromCol]->setStyleSheet("");

        if (moveResult == TurnManager::MoveResult::Success) {
            updateBoard();
            checkGameEnd();
            fromRow = fromCol = -1;

            if (turnManager.isMoveLimitExceeded()) {
                turnManager.endTurn();
                startTurn();
            } else {
                // revert to enabling only player's pieces
                if (turnManager.getCurrentPlayer() == P2_PIECE) {
                    for (int r = 0; r < BOARD_SIZE; ++r) {
                        for (int c = 0; c < BOARD_SIZE; ++c) {
                            buttons[r][c]->setEnabled(b->getPiece(r, c) == P2_PIECE);
                        }
                    }
                }
            }
        } else {
            QMessageBox::warning(this, "Invalid Move", 
                                  (moveResult == TurnManager::MoveResult::InvalidMove) 
                                  ? "Move is invalid!" 
                                  : "You cannot make more moves this turn!");

            // if player clicked same piece again, reset selection
            if (row == fromRow && col == fromCol) {
                buttons[fromRow][fromCol]->setStyleSheet("");
                fromRow = fromCol = -1;
                if (turnManager.getCurrentPlayer() == P2_PIECE) {
                    for (int r = 0; r < BOARD_SIZE; ++r) {
                        for (int c = 0; c < BOARD_SIZE; ++c) {
                            buttons[r][c]->setEnabled(b->getPiece(r, c) == P2_PIECE);
                        }
                    }
                }
            }
        }
    }
}

void GameWindow::startTurn() {
    if (turnManager.getCurrentPlayer() == P1_PIECE) {
        std::cout << "AI's Turn...\n";
        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                buttons[row][col]->setEnabled(false);
            }
        }
        aiTurn();
    } 
    else {
        std::cout << "Human's Turn...\n";
        const Board* b = turnManager.getBoard();
        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                buttons[row][col]->setEnabled(b->getPiece(row, col) == P2_PIECE);
            }
        }
    }
}

void GameWindow::aiTurn() {
    std::cout << "AI is calculating its move...\n";
    auto bestMoves = ai.findBestMoves(turnManager);

    if (bestMoves.empty()) {
        std::cout << "No valid moves executed by AI. Ending turn.\n";
        turnManager.endTurn();
        startTurn();
        return;
    }

    // make first move
    auto firstMove = bestMoves.front();
    if (turnManager.makeMove(firstMove.fromRow, firstMove.fromCol, firstMove.toRow, firstMove.toCol)
        == TurnManager::MoveResult::Success) {
        updateBoard();
        checkGameEnd();
        if (turnManager.getGameState() != Board::GameResult::Ongoing) {
            return;
        }

        // check if AI can still make another move
        if (!turnManager.isMoveLimitExceeded()) {
            auto secondMoves = ai.findBestMoves(turnManager);
            if (!secondMoves.empty()) {
                auto secondMove = secondMoves.front();
                if (turnManager.makeMove(secondMove.fromRow, secondMove.fromCol, secondMove.toRow, secondMove.toCol)
                    == TurnManager::MoveResult::Success) {
                    updateBoard();
                    checkGameEnd();
                    if (turnManager.getGameState() != Board::GameResult::Ongoing) {
                        return;
                    }
                }
            }
        }
    }

    turnManager.endTurn();
    startTurn();
}

void GameWindow::checkGameEnd() {
    auto gameState = turnManager.getGameState();
    if (gameState != Board::GameResult::Ongoing) {
        QString resultMessage;
        if (gameState == Board::GameResult::Draw) {
            resultMessage = "It's a Draw!";
        } 
        else if (gameState == Board::GameResult::Player1Wins) {
            resultMessage = "AI Wins!";
        } 
        else if (gameState == Board::GameResult::Player2Wins) {
            resultMessage = "You Win!";
        }

        QMessageBox::information(this, "Game Over", resultMessage);
        close();
    }
}
