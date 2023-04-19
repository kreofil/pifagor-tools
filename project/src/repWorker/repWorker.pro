TEMPLATE = app
CONFIG += console

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin

TARGET = repWorker

HEADERS += ../repository/quickMsg.h \
     ../repository/nameMaker.h \
    ../repository/repFacade.h \
    ../repository/msgServer.h \
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
    repWorker.h

SOURCES += repWorker.cpp \
    ../repository/quickMsg.cpp \
    ../repository/nameMaker.cpp \
    ../repository/repFacade.cpp \
    ../repository/msgServer.cpp \
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
    repWorkerMain.cpp

QT += \
    sql \
    network
