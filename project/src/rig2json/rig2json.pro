TEMPLATE = app

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_rig2cg

TARGET = rig2json

HEADERS += ../rigparser/rigparser.h \
    ../rig/value.h \
    ../rig/rig.h \
    ../rig/extref.h \
    ../rig/actor.h \
    rig2jsoner.h

SOURCES += rig2json.cpp \
    ../rigparser/rigparser.cpp \
    ../rig/value.cpp \
    ../rig/rig.cpp \
    ../rig/extref.cpp \
    ../rig/actor.cpp \
    rig2jsoner.cpp
