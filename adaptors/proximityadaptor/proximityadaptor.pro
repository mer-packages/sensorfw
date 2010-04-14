TEMPLATE     = lib
CONFIG      += plugin

TARGET       = proximityadaptor

include( ../../common-config.pri )

HEADERS += proximityadaptor.h \
           proximityadaptorplugin.h

SOURCES += proximityadaptor.cpp \
           proximityadaptorplugin.cpp

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
