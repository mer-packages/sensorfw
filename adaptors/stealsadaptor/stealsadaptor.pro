TEMPLATE     = lib
CONFIG      += plugin
TARGET       = stealsadaptor
HEADERS += stealsadaptor-sysfs.h \
           stealsadaptor-sysfsplugin.h

SOURCES += stealsadaptor-sysfs.cpp \
           stealsadaptor-sysfsplugin.cpp

include( ../adaptor-config.pri )
