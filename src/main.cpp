#include "GameWindow.h"
#include <QApplication>
#include <QTimer>
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    int depth = 3; // default depth
    for (int i = 1; i < argc; ++i) {
        if (std::string arg = argv[i]; arg == "--depth" && i + 1 < argc) {
            try {
                depth = std::stoi(argv[++i]);
            }
            catch (const std::invalid_argument&) {
                std::cerr << "Invalid depth value. Using default depth: " << depth << std::endl;
            }
        }
    }

    std::cout << "Using minimax depth: " << depth << std::endl;

    GameWindow window(nullptr, depth);
    window.setWindowTitle("Onat Ribar | CSE462 Project");

    constexpr int buttonSize = 50;
    constexpr int padding = 50;
    window.resize(BOARD_SIZE * buttonSize, BOARD_SIZE * buttonSize + padding);

    QTimer::singleShot(0, &window, &GameWindow::startTurn);

    window.show();

    return QApplication::exec();
}