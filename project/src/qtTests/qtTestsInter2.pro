#-------------------------------------------------
#
# Project created by QtCreator 2013-07-20T15:59:42
#
#-------------------------------------------------

QT       += core testlib

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin

TARGET = qtTestsInter2
CONFIG   += console
CONFIG   -= app_bundle



TEMPLATE = app

DEFINES += QT_NO_DEBUG_OUTPUT

HEADERS += ../repository/quickMsg.h \
    ../repository/const.h \
    ../repository/nameMaker.h \
    ../repository/repFacade.h \
    ../inter2/eventProcessor.h \
    ../inter2/centralManager.h \
    ../inter2/dataWorker.h \
    ../inter2/alu.h \
    ../cg/automat.h \
    ../cg/csignal.h \
    ../cg/ctrlnode.h \
    ../cg/dynlink.h \
    ../cg/cg.h \
    ../rig/rig.h \
    ../rig/extref.h \
    ../rig/actor.h \
    ../rig/value.h \
    ../rigparser/rigparser.h \
    ../cgparser/cgparser.h \
    ../globals/globals.h \
    ../linker/linker.h \
    ../repository/msgServer.h

SOURCES +=  ../repository/quickMsg.cpp \
    ../repository/nameMaker.cpp \
    ../repository/repFacade.cpp \
    qtTestsInter2.cpp \
    ../inter2/eventProcessor.cpp \
    ../inter2/centralManager.cpp  \
    ../inter2/dataWorker.cpp \
    ../inter2/alu.cpp \
    ../cg/automat.cpp \
    ../cg/csignal.cpp \
    ../cg/ctrlnode.cpp \
    ../cg/dynlink.cpp \
    ../cg/cg.cpp \
    ../rig/rig.cpp \
    ../rig/extref.cpp \
    ../rig/actor.cpp \
    ../rig/value.cpp \
    ../rigparser/rigparser.cpp \
    ../cgparser/cgparser.cpp \
    ../linker/linker.cpp \
    ../repository/msgServer.cpp

