CONFIG      += link_pkgconfig

TARGET       = gyroscopesensor

HEADERS += gyroscopesensor.h   \
           gyroscopesensor_a.h \
           gyroscopeplugin.h

SOURCES += gyroscopesensor.cpp   \
           gyroscopesensor_a.cpp \
           gyroscopeplugin.cpp

include( ../sensor-config.pri )
