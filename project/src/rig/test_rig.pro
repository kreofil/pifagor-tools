TEMPLATE = app

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_test_rig

TARGET = test_rig

HEADERS += rig.h \
    extref.h \
    actor.h \
    value.h

SOURCES += rig.cpp \
    extref.cpp \
    actor.cpp \
    value.cpp \
    test_rig.cpp
