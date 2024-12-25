// Header file: GameWindow.h
#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "TurnManager.h"
#include "Minimax.h"
#include "GameConstants.h"

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>

class GameWindow : public QMainWindow {
    Q_OBJECT

private:
    Board board;
    TurnManager turnManager;
    Minimax ai;
    QGridLayout* gridLayout;
    QPushButton* buttons[BOARD_SIZE][BOARD_SIZE]{};

    // track row and col of selected piece
    int fromRow;
    int fromCol;

    // helper methods
    void updateBoard() const;
    void enableButtonsForPlayer(int player) const;
    void handleCellClick(int row, int col);
    void selectPiece(int row, int col);
    void deselectPiece();
    void highlightValidMoves(int row, int col) const;
    void resetButtonStates();
    void checkGameEnd();
    void onGameEnd(const QString& resultMessage);
    void executeAIMove();
    void aiTurn();

public:
    explicit GameWindow(QWidget* parent = nullptr, int aiDepth = 3);
    void startTurn();
};

#endif