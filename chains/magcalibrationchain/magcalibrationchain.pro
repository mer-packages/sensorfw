TARGET  = magcalibrationchain

HEADERS += magcalibrationchain.h \
           calibrationfilter.h \
           magcalibrationchainplugin.h
 #       qvector3d.h

SOURCES += magcalibrationchain.cpp \
           calibrationfilter.cpp \
           magcalibrationchainplugin.cpp
#        qvector3d.cpp

include( ../chain-config.pri )
