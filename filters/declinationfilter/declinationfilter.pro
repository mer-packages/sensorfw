TARGET = declinationfilter

CONFIG += link_pkgconfig

PKGCONFIG += gconf-2.0 gobject-2.0

HEADERS += declinationfilter.h \
           declinationfilterplugin.h

SOURCES += declinationfilter.cpp \
           declinationfilterplugin.cpp

include( ../filter-config.pri )
