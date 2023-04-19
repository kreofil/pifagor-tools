TEMPLATE = app
CONFIG   += console

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin
#MAKEFILE = make_cgen2

TARGET = cgen2

HEADERS += ../repository/nameMaker.h \
    ../repository/repFacade.h \
    ../repository/quickMsg.h \
    ../repository/const.h \
    ../cg/automat.h \
    ../cg/csignal.h \
    ../cg/ctrlnode.h \
    ../cg/dynlink.h \
    ../cg/cg.h \
    ../rigparser/rigparser.h \
    ../rig/rig.h \
    ../rig/extref.h \
    ../rig/actor.h \
    ../rig/value.h \
    ../rig2dot/rig2dot.h \
    ../cg2dot/cg2dot.h \
    ../repository/msgServer.h

SOURCES += ../repository/nameMaker.cpp \
    ../repository/repFacade.cpp \
    ../repository/quickMsg.cpp \
    cgen2.cpp \
    ../rigparser/rigparser.cpp \
    ../cg/automat.cpp \
    ../cg/csignal.cpp \
    ../cg/ctrlnode.cpp \
    ../cg/dynlink.cpp \
    ../cg/cg.cpp \
    ../rig/rig.cpp \
    ../rig/extref.cpp \
    ../rig/actor.cpp \
    ../rig/value.cpp \
    ../rig2dot/rig2dot.cpp \
    ../cg2dot/cg2dot.cpp \
    ../repository/msgServer.cpp


