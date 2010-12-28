CONFIG      += link_pkgconfig

PKGCONFIG   += contextprovider-1.0

TARGET       = contextsensor

SENSORFW_INCLUDEPATHS = ../../

HEADERS += contextplugin.h \
           orientationbin.h \
           compassbin.h \
           stabilitybin.h \
           contextsensor.h \
           screeninterpreterfilter.h \
           normalizerfilter.h \
           avgvarfilter.h \
           cutterfilter.h \
           stabilityfilter.h \
           headingfilter.h


SOURCES += contextplugin.cpp \
           orientationbin.cpp \
           compassbin.cpp \
           stabilitybin.cpp \
           contextsensor.cpp \
           screeninterpreterfilter.cpp \
           normalizerfilter.cpp \
           avgvarfilter.cpp \
           cutterfilter.cpp \
           stabilityfilter.cpp \
           headingfilter.cpp

CONTEXT.files = 'com.nokia.SensorService.context'
CONTEXT.path = '/usr/share/contextkit/providers'
                       
INSTALLS += CONTEXT

include( ../sensor-config.pri )
