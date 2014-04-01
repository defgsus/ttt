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
QMAKE_CXXFLAGS_RELEASE += -O2 -DNDEBUG -DTTT_NO_PRINT


SOURCES += \
    main.cpp \
    engine/board.cpp \
    engine/boardhelper.cpp \
    gui/boardview.cpp \
    gui/engine.cpp \
    gui/messagebox.cpp \
    gui/particles.cpp \
    gui/mainwidget.cpp \
    gui/popbutton.cpp \
    gui/popgroup.cpp \
    gui/popwidget.cpp

HEADERS += \
    engine/board.h \
    engine/boardhelper.h \
    engine/common.h \
    engine/negamax.h \
    engine/search.h \
    gui/boardview.h \
    gui/engine.h \
    gui/messagebox.h \
    gui/particles.h \
    gui/mainwidget.h \
    gui/popbutton.h \
    gui/popgroup.h \
    gui/popwidget.h

FORMS +=

OTHER_FILES += \
    android/src/org/qtproject/qt5/android/bindings/QtActivity.java \
    android/src/org/qtproject/qt5/android/bindings/QtApplication.java \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/version.xml \
    android/AndroidManifest.xml \
    android/res/values-pl/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-es/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ro/strings.xml
