TARGET       = hybrisaccelerometeradaptor

HEADERS += hybrisaccelerometeradaptor.h \
           hybrisaccelerometeradaptorplugin.h

SOURCES += hybrisaccelerometeradaptor.cpp \
           hybrisaccelerometeradaptorplugin.cpp
LIBS+= -L../../core -lhybrissensorfw-qt5

include( ../adaptor-config.pri )
config_hybris {
    INCLUDEPATH+=/usr/include/android
}
