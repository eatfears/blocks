TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lGL -lGLU -lglut -lz -lpng

SOURCES += *.cpp

HEADERS += *.h
