TEMPLATE     = lib
CONFIG      += plugin

TARGET       = proximityadaptor-ascii

include( ../../common-config.pri )

HEADERS += proximityadaptor-ascii.h \
           proximityadaptor-asciiplugin.h

SOURCES += proximityadaptor-ascii.cpp \
           proximityadaptor-asciiplugin.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../sensord \
                        ../../datatypes \
                        ../../filters

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
