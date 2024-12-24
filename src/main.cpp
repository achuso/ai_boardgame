#include "GameWindow.h"
#include <QApplication>
#include <QTimer>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    int depth = 3; // default depth
    for (int i = 1; i < argc; ++i) {
        if (std::string arg = argv[i]; arg == "--depth" && i + 1 < argc) {
            depth = std::stoi(argv[++i]);
        }
    }

    std::cout << "Using minimax depth: " << depth << std::endl;

    GameWindow window(nullptr, depth);
    window.setWindowTitle("Onat Ribar | CSE462 Project");
    window.resize(400, 450); // Extra height for the turn indicator

    QTimer::singleShot(0, &window, &GameWindow::startTurn);

    window.show();

    return QApplication::exec();
}