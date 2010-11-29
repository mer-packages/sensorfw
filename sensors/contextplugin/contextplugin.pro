CONFIG      += link_pkgconfig

PKGCONFIG   += contextprovider-1.0

TARGET       = contextsensor

SENSORFW_INCLUDEPATHS = ../../filters/orientationinterpreter \
                        ../../filters/coordinatealignfilter \
                        ../../adaptors/sysfsadaptor \
                        ../../adaptors/accelerometeradaptor \
                        ../../adaptors/magnetometeradaptor 

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

CONTEXT.files = 'com.nokia.SensorService.context'
CONTEXT.path = '/usr/share/contextkit/providers'
                       
INSTALLS += CONTEXT

include( ../sensor-config.pri )
