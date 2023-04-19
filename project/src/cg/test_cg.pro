TEMPLATE = app

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_test_cg

TARGET = test_cg

HEADERS += cg.h \
    csignal.h \
    dynlink.h \
    ctrlnode.h \
    automat.h

SOURCES += cg.cpp \
    test_cg.cpp \
    csignal.cpp \
    dynlink.cpp \
    automat.cpp \
    ctrlnode.cpp
