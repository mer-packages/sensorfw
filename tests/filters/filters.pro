QT += dbus network

include(../common-install.pri)

TEMPLATE = app
TARGET = sensorfilters-test

CONFIG += testcase link_pkgconfig

PKGCONFIG += gconf-2.0 gobject-2.0

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

equals(QT_MAJOR_VERSION, 4):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes -lsensorfw
}

equals(QT_MAJOR_VERSION, 5):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes-qt5 -lsensorfw-qt5
}
