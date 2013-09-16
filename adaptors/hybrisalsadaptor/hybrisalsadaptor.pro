TARGET       = hybrisalsadaptor

HEADERS += hybrisalsadaptor.h \
           hybrisalsadaptorplugin.h

SOURCES += hybrisalsadaptor.cpp \
           hybrisalsadaptorplugin.cpp
LIBS+= -L../../core -lhybrissensorfw-qt5

include(../adaptor-config.pri )
