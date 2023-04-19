TEMPLATE = app

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_cg2cg

TARGET = cg2cg

HEADERS += ../cg/automat.h \
    ../cg/dynlink.h \
    ../cg/ctrlnode.h \
    ../cg/csignal.h \
    ../cg/cg.h \
    ../rigparser/rigparser.h

SOURCES += cg2cg.cpp \
    ../cg/dynlink.cpp \
    ../cg/ctrlnode.cpp \
    ../cg/csignal.cpp \
    ../cg/cg.cpp \
    ../cg/automat.cpp \
    ../cgparser/cgparser.cpp
