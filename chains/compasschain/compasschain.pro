TARGET       = compasschain

HEADERS += compasschain.h   \
           compasschainplugin.h \
           compassfilter.h \
           orientationfilter.h

SOURCES += compasschain.cpp   \
           compasschainplugin.cpp \
           compassfilter.cpp \
           orientationfilter.cpp

INCLUDEPATH += ../../filters/coordinatealignfilter \
               ../../filters/downsamplefilter \
               ../../filters/avgaccfilter \
               ../../chains/magcalibrationchain \
               ../../filters/magcoordinatealignfilter \
               ../../filters/declinationfilter

include( ../chain-config.pri )
