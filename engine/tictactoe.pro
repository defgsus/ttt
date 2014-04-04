#-------------------------------------------------
#
# Project created by QtCreator 2014-03-17T11:32:42
#
#-------------------------------------------------

#QT       += core

QT       -= gui

TARGET = tictactoe
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += --std=c++0x
QMAKE_CXXFLAGS_RELEASE += -DTTT_NO_PRINT

SOURCES += \
    main.cpp \
    board.cpp \
    boardhelper.cpp \
    board_captures.cpp \
    test_engines.cpp

HEADERS += \
    board.h \
    boardhelper.h \
    common.h \
    negamax.h \
    search.h
