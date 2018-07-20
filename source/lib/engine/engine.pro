QT       -= core gui

TARGET = engine
TEMPLATE = lib
CONFIG += console c++14
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++14

!include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

INCLUDEPATH += ../../lib/

SOURCES += \
    character.cpp \
    chunk.cpp \
    engine.cpp \
    landscape.cpp \
    light.cpp \
    material.cpp \
    platform.cpp \
    position_items.cpp \
    statistics.cpp \
    threads.cpp \
    world.cpp

HEADERS += \
    character.h \
    chunk.h \
    definitions.h \
    engine.h \
    landscape.h \
    light.h \
    material.h \
    platform.h \
    position_items.h \
    statistics.h \
    threads.h \
    world.h
