TEMPLATE = app
TARGET = ai_boardgame

QT += core widgets

CONFIG += c++17

CONFIG += outdir

INCLUDEPATH += include

SOURCES += src/main.cpp \
           src/GameWindow.cpp \
           src/Board.cpp \
           src/Minimax.cpp \
           src/TurnManager.cpp

HEADERS += include/GameWindow.h \
           include/Board.h \
           include/Minimax.h \
           include/TurnManager.h

DESTDIR = build            # Directory for the final executable
OBJECTS_DIR = build/obj    # Directory for object files (.o)
MOC_DIR = build/moc        # Directory for Qt's meta-object compiler output
UI_DIR = build/ui          # Directory for UI files
RCC_DIR = build/rcc        # Directory for Qt's resource compiler output

INCLUDEPATH += include