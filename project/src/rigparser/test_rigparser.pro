TEMPLATE = app

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_test_rig_parser

TARGET = test_rig_parser

HEADERS += rigparser.h \
    ../rig/rig.h \
    ../rig/extref.h \
    ../rig/value.h \
    ../rig/actor.h

SOURCES += test_rig_parser.cpp \
    rigparser.cpp \
    ../rig/rig.cpp \
    ../rig/extref.cpp \
    ../rig/value.cpp \
    ../rig/actor.cpp

