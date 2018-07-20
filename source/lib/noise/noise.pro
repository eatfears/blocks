QT       -= core gui

TARGET = noise
TEMPLATE = lib
CONFIG += console c++14
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++14

!include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

INCLUDEPATH += ../../lib/

SOURCES += \
    perlin_noise.cpp \
    primes.cpp

HEADERS += \
    perlin_noise.h \
    primes.h
