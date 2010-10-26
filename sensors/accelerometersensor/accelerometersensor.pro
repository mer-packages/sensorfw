TARGET       = accelerometersensor

HEADERS += accelerometersensor.h   \
           accelerometersensor_a.h \
           accelerometerplugin.h

SOURCES += accelerometersensor.cpp   \
           accelerometersensor_a.cpp \
           accelerometerplugin.cpp

include( ../sensor-config.pri )
