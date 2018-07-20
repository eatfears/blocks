CONFIG(debug, debug|release) {
    cfg=debug
    DEFINES +=_DEBUG
} else {
    cfg=release
}

rootdir=$$PWD/..
builddir=$$rootdir/.build/$$cfg

equals(TEMPLATE, app) {
    subdir=bin
}
equals(TEMPLATE, lib) {
    subdir=lib
}

bindir=$$rootdir/build/$$cfg/bin
libdir=$$rootdir/build/$$cfg/lib
DESTDIR=$$rootdir/build/$$cfg/$$subdir

HEADERS  += \
    $$rootdir/source/lib/common_include/*

PRE_TARGETDEPS += $$rootdir/source/lib/common_include/*

#QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable

OBJECTS_DIR=$$builddir/$$TARGET/objetcs
RCC_DIR=$$builddir/$$TARGET/rcc
MOC_DIR=$$builddir/$$TARGET/moc
UI_DIR=$$builddir/$$TARGET/ui
