TEMPLATE = lib
QT += dbus # TODO: remove
QT -= gui
TARGET = filters

include( ../common-config.pri )

DEPENDPATH += ..
INCLUDEPATH += ..
HEADERS += utils.h \
    timedunsigned.h \
    genericdata.h \
    orientationdata.h

SOURCES += utils.cpp

LIBS += -lrt

internal {
    HEADERS += posedata.h \
        tapdata.h \
        touchdata.h
 }

ariane {
    HEADERS += orientationdata.h \
        orientationoffsetter.h \
        genericdata.h \
        orientationconverter.h \
        timedkey.h
    SOURCES += orientationoffsetter.cpp \
        orientationconverter.cpp
}

motionband {
    HEADERS += motionbanddata.h \
        headtrackfilter.h
    SOURCES += headtrackfilter.cpp
    INCLUDEPATH += ../libs \
        ../libs/libfilter
}

include(../common-install.pri)
publicheaders.path = $${publicheaders.path}/filters
publicheaders.files = $$HEADERS

target.path = $$SHAREDLIBPATH

INSTALLS += target
