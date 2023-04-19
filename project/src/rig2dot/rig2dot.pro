TEMPLATE = app

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_rig2dot

TARGET = rig2dot

HEADERS += rig2dot.h \
    ../rig/value.h \
    ../rig/rig.h \
    ../rig/extref.h \
    ../rig/actor.h \
    ../rigparser/rigparser.h

SOURCES += rig2dotmain.cpp \
    ../rig2dot/rig2dot.cpp \
    ../rig/value.cpp \
    ../rig/rig.cpp \
    ../rig/extref.cpp \
    ../rig/actor.cpp \
    ../rigparser/rigparser.cpp
