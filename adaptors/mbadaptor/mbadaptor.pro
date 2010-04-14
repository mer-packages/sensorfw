TEMPLATE     = lib
CONFIG      += plugin
TARGET       = mbadaptor

include( ../../common-config.pri )

HEADERS += motionbandadaptor.h \
           motionbandadaptorplugin.h

SOURCES += motionbandadaptor.cpp \
           motionbandadaptorplugin.cpp

SENSORFW_INCLUDEPATHS = ../..           \
                        ../../include \
                        ../             \
                        ../../sensord   \
                        ../../datatypes \
                        ../../filters

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += ../../libs

DEFINES += USE_MOTION_BAND

LIBS += -L../../libs/ht \
        -lmotionband \
        -lmbfilter \
        -lvisualization

include(../../common-install.pri)
publicheaders.path = $${publicheaders.path}/motionbandadaptor
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
