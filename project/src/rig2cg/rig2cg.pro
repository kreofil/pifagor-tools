TEMPLATE = app

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_rig2cg

TARGET = rig2cg

HEADERS += ../cg/dynlink.h \
    ../cg/ctrlnode.h \
    ../cg/csignal.h \
    ../cg/cg.h \
    ../cg/automat.h \
    ../rigparser/rigparser.h \
    ../rig/value.h \
    ../rig/rig.h \
    ../rig/extref.h \
    ../rig/actor.h

SOURCES += rig2cg.cpp \
    ../cg/dynlink.cpp \
    ../cg/ctrlnode.cpp \
    ../cg/csignal.cpp \
    ../cg/cg.cpp \
    ../cg/automat.cpp \
    ../rigparser/rigparser.cpp \
    ../rig/value.cpp \
    ../rig/rig.cpp \
    ../rig/extref.cpp \
    ../rig/actor.cpp
