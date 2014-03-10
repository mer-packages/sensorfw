TARGET = declinationfilter

CONFIG += link_pkgconfig

PKGCONFIG += mlite5

HEADERS += declinationfilter.h \
           declinationfilterplugin.h

SOURCES += declinationfilter.cpp \
           declinationfilterplugin.cpp

include( ../filter-config.pri )
