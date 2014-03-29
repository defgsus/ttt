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
