TARGET       = alsadaptor

HEADERS += alsadaptor.h \
           alsadaptorplugin.h

SOURCES += alsadaptor.cpp \
           alsadaptorplugin.cpp

include( ../adaptor-config.pri )

!mce_disable {
    DEFINES += SENSORFW_MCE_WATCHER
}
