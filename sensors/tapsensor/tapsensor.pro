TARGET       = tapsensor

HEADERS += tapsensor.h   \
           tapsensor_a.h \
           tapplugin.h

SOURCES += tapsensor.cpp   \
           tapsensor_a.cpp \
           tapplugin.cpp

include( ../sensor-config.pri )
