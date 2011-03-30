QT += testlib

include(../common-config.pri)

QMAKE_LIBDIR_FLAGS += -L../../datatypes -L../../../datatypes -lsensordatatypes

target.path = /usr/bin
INSTALLS += target
