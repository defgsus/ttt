#-------------------------------------------------
#
# Project created by QtCreator 2014-03-28T16:46:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tictactoe_gui
TEMPLATE = app

QMAKE_CXXFLAGS += --std=c++0x -DTTT_NO_PRINT
QMAKE_CXXFLAGS_RELEASE += -O2 -DNDEBUG


SOURCES += \
    main.cpp \
    engine/board.cpp \
    engine/boardhelper.cpp \
    gui/mainwindow.cpp \
    gui/boardview.cpp \
    gui/engine.cpp

HEADERS += \
    engine/board.h \
    engine/boardhelper.h \
    engine/negamax.h \
    engine/search.h \
    gui/mainwindow.h \
    gui/boardview.h \
    gui/engine.h

FORMS += \
    gui/mainwindow.ui
