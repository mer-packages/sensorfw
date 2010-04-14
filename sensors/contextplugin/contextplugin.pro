TEMPLATE     = lib
CONFIG      += plugin link_pkgconfig

PKGCONFIG   += contextprovider-1.0

QT          += dbus

TARGET       = contextsensor

include( ../../common-config.pri )

HEADERS += contextplugin.h \
           orientationbin.h \
           contextsensor.h \
           screeninterpreterfilter.h \
           normalizerfilter.h \
           avgvarfilter.h \
           cutterfilter.h \
           stabilityfilter.h \
           compassbin.h \
           headingfilter.h

SOURCES += contextplugin.cpp \
           orientationbin.cpp \
           contextsensor.cpp \
           screeninterpreterfilter.cpp \
           normalizerfilter.cpp \
           avgvarfilter.cpp \
           cutterfilter.cpp \
           stabilityfilter.cpp \
           compassbin.cpp \
           headingfilter.cpp

SENSORFW_INCLUDEPATHS = ../..           \
                        ../../sensord   \
                        ../../datatypes \
                        ../../filters \
                        ../../filters/orientationinterpreter \
                        ../../filters/coordinatealignfilter \
                        ../../adaptors/sysfsadaptor \
                        ../../adaptors/accelerometeradaptor \
                        ../../adaptors/magnetometeradaptor \
                        ../../include

CONTEXT.files = 'com.nokia.SensorService.context'
CONTEXT.path = '/usr/share/contextkit/providers'
                       
DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target CONTEXT
