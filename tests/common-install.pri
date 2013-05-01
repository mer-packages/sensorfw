QT += testlib

include(../common-config.pri)

QMAKE_LIBDIR_FLAGS += -L../../datatypes -L../../../datatypes
equals(QT_MAJOR_VERSION, 4):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes
}
equals(QT_MAJOR_VERSION, 5):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes-qt5
}

target.path = /usr/bin
INSTALLS += target
