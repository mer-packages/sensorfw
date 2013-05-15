equals(QT_MAJOR_VERSION, 4):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes -lsensorfw
}
equals(QT_MAJOR_VERSION, 5):{
    QMAKE_LIBDIR_FLAGS += -lsensordatatypes-qt5 -lsensorfw-qt5
}

LIBS += -L/usr/local/lib -lhybris-common -lhardware
