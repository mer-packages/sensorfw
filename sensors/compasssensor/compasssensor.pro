TARGET       = compasssensor

HEADERS += compasssensor.h \
           compasssensor_a.h \
           compassplugin.h

SOURCES += compasssensor.cpp \
           compasssensor_a.cpp \
           compassplugin.cpp

include( ../sensor-config.pri )
