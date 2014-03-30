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

SOURCES += \
    main.cpp \
    board.cpp \
    boardhelper.cpp

HEADERS += \
    board.h \
    boardhelper.h \
    common.h \
    negamax.h \
    search.h
