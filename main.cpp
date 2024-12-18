#include "Board.h"
#include "Minimax.h"
#include "GameWindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Board board;
    Minimax ai;

    // AI moves first
    auto bestMove = ai.findBestMove(board);
    board.movePiece(P1_PIECE, bestMove.fromRow, bestMove.fromCol, bestMove.toRow, bestMove.toCol);

    // display the GUI after AI's first move
    GameWindow window;
    window.setWindowTitle("Strategic Board Game");
    window.resize(400, 400);
    window.show();

    return app.exec();
}
