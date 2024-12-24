#include "GameWindow.h"
#include <QMessageBox>

GameWindow::GameWindow(QWidget* parent, int aiDepth)
    : QMainWindow(parent), board(), turnManager(board), ai(aiDepth) {
    auto* centralWidget = new QWidget(this);
    gridLayout = new QGridLayout(centralWidget);

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            buttons[row][col] = new QPushButton(this);
            buttons[row][col]->setFixedSize(50, 50);
            buttons[row][col]->setEnabled(false);
            gridLayout->addWidget(buttons[row][col], row, col);

            connect(buttons[row][col], &QPushButton::clicked, [this, row, col]() {
                handleCellClick(row, col);
            });
        }
    }

    centralWidget->setLayout(gridLayout);
    setCentralWidget(centralWidget);

    updateBoard();
    startTurn();
}

void GameWindow::updateBoard() {
    const Board* b = turnManager.getBoard();
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int piece = b->getPiece(row, col);
            buttons[row][col]->setStyleSheet("");
            if (piece == EMPTY)
                buttons[row][col]->setText("");
            else if (piece == P1_PIECE)
                buttons[row][col]->setText("▲");
            else if (piece == P2_PIECE)
                buttons[row][col]->setText("●");
        }
    }
}

void GameWindow::startTurn() {
    const Board* b = turnManager.getBoard();

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (b->getPiece(row, col) == turnManager.getCurrentPlayer())
                buttons[row][col]->setEnabled(true);
            else
                buttons[row][col]->setEnabled(false);
        }
    }

    if (turnManager.getCurrentPlayer() == P1_PIECE)
        aiTurn();
}

void GameWindow::handleCellClick(const int row, const int col) {
    static int fromRow = -1, fromCol = -1;
    const Board* b = turnManager.getBoard();

    if (turnManager.hasPieceMoved(row, col)) {
        QMessageBox::warning(this, "Invalid Move", "This piece has already moved!");
        return;
    }

    if (fromRow == -1 && fromCol == -1) {
        if (b->getPiece(row, col) == turnManager.getCurrentPlayer()) {
            fromRow = row;
            fromCol = col;
            buttons[row][col]->setStyleSheet("background-color: blue;");

            for (int r = 0; r < BOARD_SIZE; ++r) {
                for (int c = 0; c < BOARD_SIZE; ++c) {
                    if (turnManager.getBoard()->isValidMove(turnManager.getCurrentPlayer(), row, col, r, c)) {
                        buttons[r][c]->setEnabled(true);
                    }
                    else {
                        buttons[r][c]->setEnabled(false);
                    }
                }
            }
            buttons[row][col]->setEnabled(true);
        }
        else {
            QMessageBox::warning(this, "Invalid Selection", "Please select one of your pieces!");
        }
    }
    else if (fromRow == row && fromCol == col) {
        buttons[fromRow][fromCol]->setStyleSheet("");
        fromRow = fromCol = -1;

        for (auto & button : buttons) {
            for (auto & c : button) {
                c->setEnabled(false);
            }
        }

        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                if (b->getPiece(r, c) == turnManager.getCurrentPlayer()) {
                    buttons[r][c]->setEnabled(true);
                }
            }
        }
    }
    else {
        auto moveResult = turnManager.makeMove(fromRow, fromCol, row, col);
        buttons[fromRow][fromCol]->setStyleSheet("");

        if (moveResult == TurnManager::MoveResult::Success) {
            updateBoard();
            checkGameEnd();
            fromRow = fromCol = -1;

            if (turnManager.isMoveLimitExceeded()) {
                turnManager.endTurn();
                startTurn();
            }
            else {
                startTurn();
            }
        }
        else {
            QMessageBox::warning(this, "Invalid Move", "This move is not allowed.");
            fromRow = fromCol = -1;
        }
    }
}

void GameWindow::aiTurn() {
    auto bestMoves = ai.findBestMoves(turnManager);
    if (!bestMoves.empty()) {
        auto firstMove = bestMoves.front();
        turnManager.makeMove(firstMove.fromRow, firstMove.fromCol, firstMove.toRow, firstMove.toCol);
        if (!turnManager.isMoveLimitExceeded() && turnManager.getGameState() == Board::GameResult::Ongoing) {
            auto secondMoves = ai.findBestMoves(turnManager);
            if (!secondMoves.empty()) {
                auto secondMove = secondMoves.front();
                turnManager.makeMove(secondMove.fromRow, secondMove.fromCol, secondMove.toRow, secondMove.toCol);
            }
        }
    }
    turnManager.endTurn();
    updateBoard();
    checkGameEnd();
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