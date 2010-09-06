TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = datatypes \
          adaptors \
          filters \
          sensors \
          sensord \
          qt-api

#sensord.depends = datatypes adaptors filters sensors chains
sensord.depends = datatypes adaptors sensors chains

include( doc/doc.pri )
include( common-install.pri )
include( common-config.pri )

PKGCONFIGFILES.files = sensord.pc
PKGCONFIGFILES.path = /usr/lib/pkgconfig

QTCONFIGFILES.files = sensord.prf
QTCONFIGFILES.path = /usr/share/qt4/mkspecs/features

DBUSCONFIGFILES.files = sensorfw.conf
DBUSCONFIGFILES.path = /etc/dbus-1/system.d

SENSORDCONFIGFILES.files = config/sensord.conf
SENSORDCONFIGFILES.path = /etc

internal {
    SUBDIRS += chains tests
}

publicheaders.files += include/*.h

INSTALLS += PKGCONFIGFILES QTCONFIGFILES DBUSCONFIGFILES SENSORDCONFIGFILES
