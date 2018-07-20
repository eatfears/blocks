QT       -= core gui

TARGET = logger
TEMPLATE = lib
CONFIG += console c++14
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++14

!include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

INCLUDEPATH += ../../lib/

SOURCES += \
    logger.cpp

HEADERS += \
    logger.h
