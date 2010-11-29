TARGET       = proximitysensor

HEADERS += proximitysensor.h   \
           proximitysensor_a.h \
           proximityplugin.h

SOURCES += proximitysensor.cpp   \
           proximitysensor_a.cpp \
           proximityplugin.cpp

include( ../sensor-config.pri )
