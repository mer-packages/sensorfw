TARGET       = hybrisgyroscopeadaptor

HEADERS += hybrisgyroscopeadaptor.h \
           hybrisgyroscopeadaptorplugin.h

SOURCES += hybrisgyroscopeadaptor.cpp \
           hybrisgyroscopeadaptorplugin.cpp

LIBS+= -L../../core -lhybrissensorfw-qt5

include( ../adaptor-config.pri )
config_hybris {
    INCLUDEPATH+=/usr/include/android
}
