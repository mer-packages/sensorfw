TARGET       = rotationsensor

HEADERS += rotationsensor.h   \
           rotationsensor_a.h \
           rotationplugin.h

SOURCES += rotationsensor.cpp   \
           rotationsensor_a.cpp \
           rotationplugin.cpp

include( ../sensor-config.pri )
