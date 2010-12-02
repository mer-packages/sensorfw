CONFIG      += link_pkgconfig

TARGET       = alssensor

HEADERS += alssensor.h   \
           alssensor_a.h \
           alsplugin.h

SOURCES += alssensor.cpp   \
           alssensor_a.cpp \
           alsplugin.cpp

include( ../sensor-config.pri )

contextprovider {
    DEFINES += PROVIDE_CONTEXT_INFO
    PKGCONFIG += contextprovider-1.0
}

