TEMPLATE = lib
CONFIG += plugin link_pkgconfig

QT += dbus
QT -= gui

PKGCONFIG   += gconf-2.0

TARGET = declinationfilter

include( ../../common-config.pri )

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        .. \
                        ../../sensord \
                        ../../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

HEADERS += declinationfilter.h \
           declinationfilterplugin.h

SOURCES += declinationfilter.cpp \
           declinationfilterplugin.cpp

include(../../common-install.pri)
publicheaders.files = $$HEADERS

target.path = $$PLUGINPATH
INSTALLS += target
