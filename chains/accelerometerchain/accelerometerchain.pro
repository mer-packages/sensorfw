TARGET       = accelerometerchain

HEADERS += accelerometerchain.h   \
           accelerometerchainplugin.h

SOURCES += accelerometerchain.cpp   \
           accelerometerchainplugin.cpp

INCLUDEPATH += ../../filters/coordinatealignfilter

include( ../chain-config.pri )
