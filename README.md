# CSE 462 - Strategic Board Game Project

This project implements a simple 7x7 strategic board game with a Minimax-based AI opponent, developed for the CSE462 - Fall 2024 course.

Game rules can be found within the repository in the document `Term Project.pdf`.

## Running the Project

### Prerequisites
- C++17 or newer
- Qt5 or Qt6
- A C++ compiler (e.g., g++ or clang++)

### Build and Run
1. Use `qmake` and `cmake` to generate build files:
   ```bash
   qmake
   make
   ```

2. Run the executable:
    ```bash
    ./build/ai_boardgame [--depth X]
    ```
    where X is an optional depth parameter for the Minimax search algorithm (default=3).
