TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = datatypes \ 
          adaptors \
          filters \
          sensors \
          core \
          sensord \
          qt-api \
          chains \
          tests \
          examples

# Until the situation regarding contextkit etc is sorted out, we don't want to
# port the daemon to Qt 5. So we build it only for Qt 4, and depend on it for
# the Qt 5 package.
#
# When that is sorted out, we can reverse the situation, and only build the
# daemon for Qt 5. There is no sense in having a co-installable daemon.
equals(QT_MAJOR_VERSION, 5): {
    SUBDIRS = datatypes qt-api
}

qt-api.depends = datatypes
sensord.depends = datatypes adaptors sensors chains

#include( doc/doc.pri )
include( common-install.pri )
include( common-config.pri )

equals(QT_MAJOR_VERSION, 4):{
    PKGCONFIGFILES.files = sensord.pc
    QTCONFIGFILES.path = /usr/share/qt4/mkspecs/features
}

equals(QT_MAJOR_VERSION, 5):{
    PKGCONFIGFILES.files = sensord-qt5.pc
    QTCONFIGFILES.path = /usr/share/qt5/mkspecs/features

}

PKGCONFIGFILES.path = /usr/lib/pkgconfig

# How to make this work in all cases?
#PKGCONFIGFILES.commands = sed -i \"s/Version:.*$$/Version: `head -n1 debian/changelog | cut -f 2 -d\' \' | tr -d \'()\'`/\" sensord.pc

QTCONFIGFILES.files = sensord.prf

equals(QT_MAJOR_VERSION, 4):  {
    DBUSCONFIGFILES.files = sensorfw.conf
    DBUSCONFIGFILES.path = /etc/dbus-1/system.d

    SENSORDCONFIGFILE.files = config/sensor*.conf
    SENSORDCONFIGFILE.path = /etc/sensorfw

    SENSORDCONFIGFILES.files = config/90-sensord-default.conf
    SENSORDCONFIGFILES.path = /etc/sensorfw/sensord.conf.d

    INSTALLS += DBUSCONFIGFILES SENSORDCONFIGFILE SENSORDCONFIGFILES
}

publicheaders.files += include/*.h

INSTALLS += PKGCONFIGFILES QTCONFIGFILES

equals(QT_MAJOR_VERSION, 4):  {
    OTHER_FILES += rpm/sensorfw.spec \
                   rpm/sensorfw.yaml
}

equals(QT_MAJOR_VERSION, 5):  {
    OTHER_FILES += rpm/sensorfw-qt5.spec \
                   rpm/sensorfw-qt5.yaml
}

OTHER_FILES += config/*
