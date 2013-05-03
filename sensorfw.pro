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

DBUSCONFIGFILES.files = sensorfw.conf
DBUSCONFIGFILES.path = /etc/dbus-1/system.d

SENSORDCONFIGFILE.files = config/sensor*.conf
SENSORDCONFIGFILE.path = /etc/sensorfw

SENSORDCONFIGFILES.files = config/90-sensord-default.conf
SENSORDCONFIGFILES.path = /etc/sensorfw/sensord.conf.d

publicheaders.files += include/*.h

INSTALLS += PKGCONFIGFILES QTCONFIGFILES DBUSCONFIGFILES SENSORDCONFIGFILE SENSORDCONFIGFILES

equals(QT_MAJOR_VERSION, 4):  {
    OTHER_FILES += rpm/sensorfw.spec \
                   rpm/sensorfw.yaml
}

equals(QT_MAJOR_VERSION, 5):  {
    OTHER_FILES += rpm/sensorfw-qt5.spec \
                   rpm/sensorfw-qt5.yaml
}
