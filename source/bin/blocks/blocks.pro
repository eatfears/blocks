TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

!include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QMAKE_CXXFLAGS += -std=c++14
QMAKE_LFLAGS += -static-libstdc++

INCLUDEPATH += ../../lib/

LIBS += -L"$$libdir"
LIBS += -lengine -llogger -lnoise
LIBS += -lGL -lGLU -lglut -lz -lpng -lpthread

#PRE_TARGETDEPS += $$libdir/libengine.so
#PRE_TARGETDEPS += $$libdir/liblogger.so
#PRE_TARGETDEPS += $$libdir/libnoise.so

SOURCES += \
    main.cpp

HEADERS +=
