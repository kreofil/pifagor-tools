TEMPLATE = app

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_rig2rig

TARGET = rig2rig

HEADERS += ../rigparser/rigparser.h \
    ../rig/rig.h \
    ../rig/extref.h \
    ../rig/value.h \
    ../rig/actor.h

SOURCES += rig2rig.cpp \
    ../rigparser/rigparser.cpp \
    ../rig/rig.cpp \
    ../rig/extref.cpp \
    ../rig/value.cpp \
    ../rig/actor.cpp
