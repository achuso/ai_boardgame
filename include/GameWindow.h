#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>

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
    QPushButton* buttons[BOARD_SIZE][BOARD_SIZE]{};
    QPushButton* startButton{};

    // helper methods
    void updateBoard();
    void handleCellClick(int row, int col);
    void checkGameEnd();
    void disableAllButtons();
    void aiTurn();

public:
    explicit GameWindow(QWidget* parent = nullptr, int aiDepth = 3);
    void startTurn();
};

#endif
