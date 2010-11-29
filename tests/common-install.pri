QT += testlib

include(../common-config.pri)

LIBS += -L../../datatypes -L../../../datatypes -lsensordatatypes

target.path = /usr/bin
INSTALLS += target
