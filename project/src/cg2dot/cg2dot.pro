TEMPLATE = app

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_cg2dot

TARGET = cg2dot

HEADERS += cg2dot.h \
    ../cg/dynlink.h \
    ../cg/ctrlnode.h \
    ../cg/csignal.h \
    ../cg/cg.h \
    ../cg/automat.h \
    ../cgparser/cgparser.h

SOURCES += cg2dotmain.cpp \
    cg2dot.cpp \
    ../cg/dynlink.cpp \
    ../cg/ctrlnode.cpp \
    ../cg/csignal.cpp \
    ../cg/cg.cpp \
    ../cg/automat.cpp \
    ../cgparser/cgparser.cpp
