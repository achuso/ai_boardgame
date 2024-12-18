#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include "Board.h"
#include "Minimax.h"

class GameWindow : public QMainWindow {
    Q_OBJECT

private:
    Board board;                  // Game board
    Minimax ai;                   // AI player
    QGridLayout* gridLayout;      // Layout for the board
    QPushButton* buttons[7][7];   // Buttons for the 7x7 grid
    int currentPlayer = P2_PIECE; // Start with human player

    // Helper to update button visuals based on board state
    void updateBoard();

    // Handle player moves
    void handleCellClick(int row, int col);

public:
    GameWindow(QWidget* parent = nullptr);
};

#endif
