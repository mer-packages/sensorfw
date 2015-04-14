TARGET  = magcalibrationchain

HEADERS += magcalibrationchain.h \
           calibrationfilter.h \
           magcalibrationchainplugin.h
 #       qvector3d.h

SOURCES += magcalibrationchain.cpp \
           calibrationfilter.cpp \
           magcalibrationchainplugin.cpp
#        qvector3d.cpp

INCLUDEPATH += ../../filters/magcoordinatealignfilter

include( ../chain-config.pri )
