TARGET       = compasschain

HEADERS += compasschain.h   \
           compasschainplugin.h \
           compassfilter.h

SOURCES += compasschain.cpp   \
           compasschainplugin.cpp \
           compassfilter.cpp

#HEADERS += ../../filters/avgaccfilter/avgaccfilter.h
#SOURCES += ../../filters/avgaccfilter/avgaccfilter.cpp

INCLUDEPATH += ../../filters/coordinatealignfilter
#INCLUDEPATH += ../../filters/avgaccfilter

include( ../chain-config.pri )
