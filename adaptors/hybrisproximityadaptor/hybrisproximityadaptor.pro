TARGET       = hybrisproximityadaptor

HEADERS += hybrisproximityadaptor.h \
           hybrisproximityadaptorplugin.h

SOURCES += hybrisproximityadaptor.cpp \
           hybrisproximityadaptorplugin.cpp

LIBS+= -L../../core -lhybrissensorfw-qt5

include( ../adaptor-config.pri )
    PKGCONFIG += android-headers
    PKGCONFIG += libhardware
