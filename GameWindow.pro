TEMPLATE = app
TARGET = ai_boardgame

QT += core widgets

CONFIG += c++17

SOURCES += main.cpp \
           GameWindow.cpp \
           Board.cpp \
           Minimax.cpp \
           TurnManager.cpp

HEADERS += GameWindow.h \
           Board.h \
           Minimax.h \
           TurnManager.h