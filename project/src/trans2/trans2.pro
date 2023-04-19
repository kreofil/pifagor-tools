CONFIG   += console
TEMPLATE = app

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin

TARGET = trans2

HEADERS += ../repository/nameMaker.h \
    ../repository/repFacade.h \
    ../repository/msgServer.h \
    ../repository/quickMsg.h \
    ../repository/const.h \
    pfg_scanner2.h \
    pfg_parser2.h \
    ../rig/rig.h \
    ../rig/extref.h \
    ../rig/actor.h \
    ../rig/value.h \
    ../rig2dot/rig2dot.h



SOURCES += ../repository/nameMaker.cpp \
    ../repository/repFacade.cpp \
    ../repository/msgServer.cpp \
    ../repository/quickMsg.cpp \
    trans2.cpp \
    pfg_scanner2.cpp \
    pfg_parser2.cpp \
    ../rig/rig.cpp \
    ../rig/extref.cpp \
    ../rig/actor.cpp \
    ../rig/value.cpp \
    ../rig2dot/rig2dot.cpp

