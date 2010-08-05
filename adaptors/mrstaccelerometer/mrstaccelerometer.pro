TEMPLATE     = lib
CONFIG      += plugin

TARGET       = mrstaccelerometeradaptor-mrst

include( ../../common-config.pri )

HEADERS += mrstaccelerometeradaptor.h \
           mrstaccelerometeradaptorplugin.h

SOURCES += mrstaccelerometeradaptor.cpp \
           mrstaccelerometeradaptorplugin.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../ \
                        ../../sensord \
                        ../../datatypes \
                        ../../filters 

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
