TEMPLATE = app

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_test_cgparser

TARGET = test_cgparser

HEADERS += /cgparser.h \
    ../cg/dynlink.h \
    ../cg/ctrlnode.h \
    ../cg/csignal.h \
    ../cg/automat.h

SOURCES += cgparser.cpp \
    test_cg_parser.cpp \
    ../cg/dynlink.cpp \
    ../cg/ctrlnode.cpp \
    ../cg/csignal.cpp \
    ../cg/cg.cpp \
    ../cg/automat.cpp
