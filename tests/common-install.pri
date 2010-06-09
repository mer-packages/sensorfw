
QT += testlib
QT -= gui 

include(../common-config.pri)

QMAKE_LIBDIR_FLAGS += -L../../datatypes -lsensordatatypes

#target.path = /usr/lib/sensord-tests
target.path = /usr/bin
INSTALLS += target
