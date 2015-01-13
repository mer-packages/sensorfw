TEMPLATE     = lib
CONFIG      += plugin

include( ../common-config.pri )

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../core \
                        ../../datatypes

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target

