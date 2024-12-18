#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QVBoxLayout>

#include "TurnManager.h"
#include "Minimax.h"
#include "GameConstants.h"

class GameWindow : public QMainWindow {
    Q_OBJECT

private:
    Board board;
    TurnManager turnManager;
    Minimax ai;
    QGridLayout* gridLayout;
    QPushButton* buttons[BOARD_SIZE][BOARD_SIZE];

    void updateBoard();
    void handleCellClick(int row, int col);
    void checkGameEnd();

public:
    GameWindow(QWidget* parent = nullptr);
    void startTurn();
    void aiTurn();
};

#endif