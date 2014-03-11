QT += dbus network

include(../common-install.pri)

TEMPLATE = app
TARGET = sensorfilters-test

CONFIG += testcase link_pkgconfig

PKGCONFIG += mlite5

HEADERS += filtertests.h \
    ../../filters/orientationinterpreter/orientationinterpreter.h \
    ../../filters/coordinatealignfilter/coordinatealignfilter.h \
    ../../filters/declinationfilter/declinationfilter.h \
    ../../filters/rotationfilter/rotationfilter.h 

    
SOURCES += filtertests.cpp \
    ../../filters/orientationinterpreter/orientationinterpreter.cpp \
    ../../filters/coordinatealignfilter/coordinatealignfilter.cpp \
    ../../filters/declinationfilter/declinationfilter.cpp \
    ../../filters/rotationfilter/rotationfilter.cpp

INCLUDEPATH += ../../include \
    ../../ \
    ../../filters/orientationinterpreter \
    ../../filters/coordinatealignfilter \
    ../../filters/declinationfilter \
    ../../filters/rotationfilter \
    ../../core \
    ../../datatypes
    
QMAKE_LIBDIR_FLAGS += -L../../datatypes
QMAKE_LIBDIR_FLAGS += -L../../builddir/core -L../../core/

include(../../common.pri)
