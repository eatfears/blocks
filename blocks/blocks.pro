TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lGL -lGLU -lglut -lz -lpng

SOURCES += \
    character.cpp \
    chunk.cpp \
    engine.cpp \
    landscape.cpp \
    light.cpp \
    main.cpp \
    material.cpp \
    perlin_noise.cpp \
    platform.cpp \
    position_items.cpp \
    primes.cpp \
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
    perlin_noise.h \
    platform.h \
    position_items.h \
    primes.h \
    statistics.h \
    threads.h \
    world.h
