#include "Board.h"
#include "Minimax.h"
#include "GameWindow.h"

#include <QApplication>
#include <QTimer>

#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    int depth = 3; // default
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--depth" && i + 1 < argc) {
            depth = std::stoi(argv[++i]);
        }
    }

    std::cout << "Using minimax depth: " << depth << std::endl;

    GameWindow window(nullptr, depth);
    window.setWindowTitle("Onat Ribar | CSE462 Project");
    window.resize(400, 400);

    // schedule AI turn after GUI is initialized if needed
    QTimer::singleShot(0, &window, &GameWindow::startTurn);

    window.show();

    return app.exec();
}
