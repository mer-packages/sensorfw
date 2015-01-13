TARGET       = hybrisproximityadaptor

HEADERS += hybrisproximityadaptor.h \
           hybrisproximityadaptorplugin.h

SOURCES += hybrisproximityadaptor.cpp \
           hybrisproximityadaptorplugin.cpp

LIBS+= -L../../core -lhybrissensorfw-qt5

include( ../adaptor-config.pri )
config_hybris {
    INCLUDEPATH+=/usr/include/android
}
