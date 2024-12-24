#include "GameWindow.h"
#include <QMessageBox>
#include <QTimer>

GameWindow::GameWindow(QWidget* parent, int aiDepth)
    : QMainWindow(parent), board(), turnManager(board), ai(aiDepth), aiThread(nullptr) {
    auto* centralWidget = new QWidget(this);
    gridLayout = new QGridLayout(centralWidget);

    // turn indicator
    turnIndicator = new QLabel("Press Start to Begin", this);
    turnIndicator->setAlignment(Qt::AlignCenter);
    turnIndicator->setStyleSheet("font-size: 16px; font-weight: bold; color: white; background-color: black; padding: 5px;");
    gridLayout->addWidget(turnIndicator, BOARD_SIZE, 0, 1, BOARD_SIZE);

    // start button
    startButton = new QPushButton("Start Game", this);
    gridLayout->addWidget(startButton, BOARD_SIZE + 1, 0, 1, BOARD_SIZE);
    connect(startButton, &QPushButton::clicked, [this]() {
        startButton->setDisabled(true);
        startButton->hide();
        turnIndicator->setText("Player Turn: Human");
        startTurn();   // Begin the game only after this button is clicked
    });

    // grid layout for board cells
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

    centralWidget->setLayout(gridLayout);
    setCentralWidget(centralWidget);

    // render the initial board configuration
    updateBoard();
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

void GameWindow::handleCellClick(const int row, const int col) {
    static int fromRow = -1, fromCol = -1;
    const Board* b = turnManager.getBoard();

    if (fromRow == -1 && fromCol == -1) {
        // select piece
        if (b->getPiece(row, col) == turnManager.getCurrentPlayer()) {
            fromRow = row;
            fromCol = col;
            buttons[row][col]->setStyleSheet("background-color: blue;"); // Highlight selected piece

            // enable all possible destination cells for this piece
            for (int r = 0; r < BOARD_SIZE; ++r) {
                for (int c = 0; c < BOARD_SIZE; ++c) {
                    buttons[r][c]->setEnabled(turnManager.getBoard()->isValidMove(turnManager.getCurrentPlayer(), row, col, r, c));
                }
            }
        }
        else {
            QMessageBox::warning(this, "Invalid Selection", "Please select your own piece!");
        }
    }
    else {
        // attempt move
        auto moveResult = turnManager.makeMove(fromRow, fromCol, row, col);
        buttons[fromRow][fromCol]->setStyleSheet(""); // reset style of the selected piece

        if (moveResult == TurnManager::MoveResult::Success) {
            updateBoard();
            checkGameEnd();
            fromRow = fromCol = -1; // Reset selection

            if (turnManager.isMoveLimitExceeded()) {
                turnManager.endTurn();
                startTurn();
            }
            else {
                // re-enable only human pieces if another move is allowed
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
            QMessageBox::warning(this, "Invalid Move",
                moveResult == TurnManager::MoveResult::InvalidMove ? "Move is invalid!" : "Move limit exceeded!");
            fromRow = fromCol = -1; // Reset selection
        }
    }
}

void GameWindow::startTurn() {
    if (turnManager.getCurrentPlayer() == P1_PIECE) {
        // AI's turn
        turnIndicator->setText("AI is Thinking...");
        for (auto & button : buttons) {
            for (auto & col : button) {
                col->setEnabled(false); // Disable human input
            }
        }
        aiTurn();
    }
    else {
        // Human's turn
        turnIndicator->setText("Player Turn: Human");
        const Board* b = turnManager.getBoard();
        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                buttons[row][col]->setEnabled(b->getPiece(row, col) == P2_PIECE); // Enable only human pieces
            }
        }
    }
}

void GameWindow::aiTurn() {
    aiThread = QThread::create([this]() {
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
    });

    connect(aiThread, &QThread::finished, this, &GameWindow::onAICompleted);
    aiThread->start();
}

void GameWindow::onAICompleted() {
    aiThread->deleteLater();
    aiThread = nullptr;

    updateBoard();
    checkGameEnd();

    if (turnManager.getGameState() == Board::GameResult::Ongoing) {
        startTurn();
    }
}

void GameWindow::checkGameEnd() {
    auto gameState = turnManager.getGameState();
    if (gameState != Board::GameResult::Ongoing) {
        QString resultMessage;
        if (gameState == Board::GameResult::Draw)
            resultMessage = "It's a Draw!";
        else if (gameState == Board::GameResult::Player1Wins)
            resultMessage = "AI Wins!";
        else if (gameState == Board::GameResult::Player2Wins)
            resultMessage = "You Win!";

        QMessageBox::information(this, "Game Over", resultMessage);
        close();
    }
}