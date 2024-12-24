#include "GameWindow.h"

GameWindow::GameWindow(QWidget* parent, int aiDepth)
    : QMainWindow(parent), board(), turnManager(board), ai(aiDepth), fromRow(-1), fromCol(-1) {

    auto* centralWidget = new QWidget(this);
    gridLayout = new QGridLayout();

    auto* layout = new QVBoxLayout();
    layout->addLayout(gridLayout);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

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

void GameWindow::enableButtonsForPlayer(int player) {
    const Board* b = turnManager.getBoard();
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            buttons[row][col]->setEnabled(b->getPiece(row, col) == player);
        }
    }
}

void GameWindow::handleCellClick(const int row, const int col) {
    if (turnManager.hasPieceMoved(row, col)) {
        QMessageBox::warning(this, "Invalid Move", "This piece has already moved!");
        return;
    }

    if (fromRow == -1 && fromCol == -1) {
        if (turnManager.getBoard()->getPiece(row, col) == turnManager.getCurrentPlayer()) {
            selectPiece(row, col);
        }
        else {
            QMessageBox::warning(this, "Invalid Selection", "Please select one of your pieces!");
        }
    }
    else if (fromRow == row && fromCol == col) {
        deselectPiece();
    }
    else {
        auto moveResult = turnManager.makeMove(fromRow, fromCol, row, col);
        deselectPiece();

        if (moveResult == TurnManager::MoveResult::Success) {
            updateBoard();
            checkGameEnd();
            if (turnManager.isMoveLimitExceeded()) {
                turnManager.endTurn();
            }
            startTurn();
        }
        else {
            QMessageBox::warning(this, "Invalid Move", "This move is not allowed.");
        }
    }
}

void GameWindow::selectPiece(int row, int col) {
    fromRow = row;
    fromCol = col;
    buttons[row][col]->setStyleSheet("background-color: blue;");
    highlightValidMoves(row, col);
}

void GameWindow::deselectPiece() {
    buttons[fromRow][fromCol]->setStyleSheet("");
    fromRow = fromCol = -1;
    resetButtonStates();
    enableButtonsForPlayer(turnManager.getCurrentPlayer());
}

void GameWindow::highlightValidMoves(int row, int col) const {
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            buttons[r][c]->setEnabled(
                turnManager.getBoard()->isValidMove(turnManager.getCurrentPlayer(), row, col, r, c)
            );
        }
    }
    buttons[row][col]->setEnabled(true); // Keep the selected piece enabled
}

void GameWindow::resetButtonStates() {
    for (auto& buttonRow : buttons) {
        for (auto* button : buttonRow) {
            button->setEnabled(false);
        }
    }
}

void GameWindow::startTurn() {
    enableButtonsForPlayer(turnManager.getCurrentPlayer());
    if (turnManager.getCurrentPlayer() == P1_PIECE) {
        aiTurn();
    }
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

        onGameEnd(resultMessage);
    }
}

void GameWindow::onGameEnd(const QString& resultMessage) {
    QMessageBox::information(this, "Game Over", resultMessage);
    close();
}

void GameWindow::executeAIMove() {
    auto bestMoves = ai.findBestMoves(turnManager);
    if (!bestMoves.empty()) {
        auto move = bestMoves.front();
        turnManager.makeMove(move.fromRow, move.fromCol, move.toRow, move.toCol);
    }
}

void GameWindow::aiTurn() {
    executeAIMove();
    if (!turnManager.isMoveLimitExceeded() && turnManager.getGameState() == Board::GameResult::Ongoing) {
        executeAIMove();
    }
    turnManager.endTurn();
    updateBoard();
    checkGameEnd();
    startTurn();
}