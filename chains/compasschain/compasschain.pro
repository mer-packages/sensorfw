TARGET       = compasschain

HEADERS += compasschain.h   \
           compasschainplugin.h \
           compassfilter.h \
           orientationfilter.h

SOURCES += compasschain.cpp   \
           compasschainplugin.cpp \
           compassfilter.cpp \
           orientationfilter.cpp

INCLUDEPATH += ../../filters/coordinatealignfilter

include( ../chain-config.pri )
