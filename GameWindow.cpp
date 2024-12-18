#include "GameWindow.h"
#include <QMessageBox>
#include <iostream>

GameWindow::GameWindow(QWidget* parent)
    : QMainWindow(parent), board(), turnManager(board), ai(3) {
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

    // if no piece is currently selected
    if (fromRow == -1 && fromCol == -1) {
        // must select your own piece first
        if (b->getPiece(row, col) == turnManager.getCurrentPlayer()) {
            fromRow = row;
            fromCol = col;
            buttons[row][col]->setStyleSheet("background-color: blue;");

            // enable the entire board so player can choose a destination
            for (int r = 0; r < BOARD_SIZE; ++r) {
                for (int c = 0; c < BOARD_SIZE; ++c) {
                    buttons[r][c]->setEnabled(true);
                }
            }

        } 
        else {
            QMessageBox::warning(this, "Invalid Selection", "Please select your own piece!");
        }

    } 
    else {
        // we have a piece selected, now trying to move it
        auto moveResult = turnManager.makeMove(fromRow, fromCol, row, col);
        buttons[fromRow][fromCol]->setStyleSheet(""); // reset highlight

        if (moveResult == TurnManager::MoveResult::Success) {
            updateBoard();

            fromRow = fromCol = -1;

            // if move limit is exceeded or turn should end
            if (turnManager.isMoveLimitExceeded()) {
                turnManager.endTurn();
                startTurn();
            } 
            else {
                // the turn isn't over yet, so disable everything except player's pieces again
                // to choose the next piece for the next move
                if (turnManager.getCurrentPlayer() == P2_PIECE) {
                    for (int r = 0; r < BOARD_SIZE; ++r) {
                        for (int c = 0; c < BOARD_SIZE; ++c) {
                            buttons[r][c]->setEnabled(b->getPiece(r, c) == P2_PIECE);
                        }
                    }
                }
            }

        } 
        else {
            // move failed, show warning and dont revert to only player's pieces.
            // keep the board fully enabled so player can try another cell.
            QMessageBox::warning(this, "Invalid Move",
                                  moveResult == TurnManager::MoveResult::InvalidMove
                                  ? "Move is invalid!"
                                  : "You cannot make more moves this turn!");

            // since it failed, we still have a piece selected. do not reset fromRow/fromCol
            // so the player can try another destination
            if (row == fromRow && col == fromCol) {
                // Player clicked the same piece again, reset the selection
                buttons[fromRow][fromCol]->setStyleSheet("");
                fromRow = fromCol = -1;

                // re-enable only player's pieces after canceling selection
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
        std::cout << "AI's Turn..." << std::endl;

        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                buttons[row][col]->setEnabled(false);
            }
        }

        aiTurn();
    } 
    else {
        std::cout << "Human's Turn..." << std::endl;

        const Board* b = turnManager.getBoard();
        // enable only the human player's pieces to start
        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                if (b->getPiece(row, col) == P2_PIECE) {
                    buttons[row][col]->setEnabled(true);
                } else {
                    buttons[row][col]->setEnabled(false);
                }
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
            // try another move
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

    // end turn after attempting up to two moves
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
