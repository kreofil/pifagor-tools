TEMPLATE = app
CONFIG += console
CONFIG += c++11
CONFIG -= app_bundle
CONFIG += qt

OBJECTS_DIR = ../../_build/obj
DESTDIR  = ../../bin

QT += core5compat

TARGET = optimize

SOURCES += main.cpp \
    bif.cpp \
    dfg_import_export.cpp \
    optimization/dead_code.cpp \
    dfg_functions.cpp \
    optimization/equivalent_transformations.cpp \
    optimization/inline_substitution.cpp \
    optimization/repeated_code.cpp \
    optimization/parlist_invariant.cpp \
    optimization/recursive_invariant.cpp


HEADERS += \
    dfg.h \
    bif.h \
    optimization/methods.h \
    dfg_functions.h \
    invariant_functions.h
