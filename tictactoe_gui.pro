#-------------------------------------------------
#
# Project created by QtCreator 2014-03-28T16:46:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tictactoe_gui
TEMPLATE = app

QMAKE_CXXFLAGS += --std=c++0x
QMAKE_CXXFLAGS_RELEASE += -O2 -DNDEBUG


SOURCES += \
    main.cpp \
    engine/board.cpp \
    engine/boardhelper.cpp \
    engine/search.cpp \
    gui/mainwindow.cpp \
    gui/boardview.cpp

HEADERS += \
    engine/board.h \
    engine/boardhelper.h \
    engine/search.h \
    gui/mainwindow.h \
    gui/boardview.h

FORMS += \
    gui/mainwindow.ui
