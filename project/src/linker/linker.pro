TEMPLATE = app
CONFIG   += console

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_linker

TARGET = link

HEADERS += linker.h \
    ../repository/quickMsg.h \
    ../repository/nameMaker.h \
    ../repository/repFacade.h \
    ../repository/msgServer.h \
    ../cg/automat.h \
    ../cg/csignal.h \
    ../cg/ctrlnode.h \
    ../cg/dynlink.h \
    ../cg/cg.h \
    ../rigparser/rigparser.h \
    ../cgparser/cgparser.h \
    ../rig/rig.h \
    ../rig/extref.h \
    ../rig/actor.h \
    ../rig/value.h

SOURCES += linker.cpp \
    link.cpp \
     ../repository/quickMsg.cpp \
     ../repository/nameMaker.cpp \
    ../repository/repFacade.cpp \
    ../repository/msgServer.cpp \
    ../rigparser/rigparser.cpp \
    ../cgparser/cgparser.cpp \
    ../cg/automat.cpp \
    ../cg/csignal.cpp \
    ../cg/ctrlnode.cpp \
    ../cg/dynlink.cpp \
    ../cg/cg.cpp \
    ../rig/rig.cpp \
    ../rig/extref.cpp \
    ../rig/actor.cpp \
    ../rig/value.cpp

QT += \
    sql \
    network
