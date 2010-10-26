TARGET       = magnetometersensor

HEADERS += magnetometersensor.h   \
           magnetometersensor_a.h \
           magnetometerscalefilter.h \
           magnetometerplugin.h

SOURCES += magnetometersensor.cpp   \
           magnetometersensor_a.cpp \
           magnetometerscalefilter.cpp \
           magnetometerplugin.cpp

include( ../sensor-config.pri )
