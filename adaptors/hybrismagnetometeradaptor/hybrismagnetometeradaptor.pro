TARGET       = hybrismagnetometeradaptor

HEADERS += hybrismagnetometeradaptor.h \
           hybrismagnetometeradaptorplugin.h

SOURCES += hybrismagnetometeradaptor.cpp \
           hybrismagnetometeradaptorplugin.cpp

LIBS+= -L../../core -lhybrissensorfw-qt5

include( ../adaptor-config.pri )
