load(configure)
qtCompileTest(hybris)

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = datatypes \ 
          adaptors \
          core \
          filters \
          sensors \
          sensord \
          qt-api \
          chains \
          tests \
          examples

equals(QT_MAJOR_VERSION, 4): {
    SUBDIRS = datatypes qt-api
}

contains(CONFIG,hybris) {

    SUBDIRS = core/hybris.pro \
               adaptors
} else {
    config_hybris {
    SUBDIRS += core/hybris.pro \
               adaptors
    }
    publicheaders.files += include/*.h

    INSTALLS += PKGCONFIGFILES QTCONFIGFILES
    PKGCONFIGFILES.path = /usr/lib/pkgconfig
    QTCONFIGFILES.files = sensord.prf

    qt-api.depends = datatypes
    sensord.depends = datatypes adaptors sensors chains

    include( doc/doc.pri )
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
}



# How to make this work in all cases?
#PKGCONFIGFILES.commands = sed -i \"s/Version:.*$$/Version: `head -n1 debian/changelog | cut -f 2 -d\' \' | tr -d \'()\'`/\" sensord.pc


equals(QT_MAJOR_VERSION, 5):  {
    !contains(CONFIG,hybris) {
# config file installation not handled here
        DBUSCONFIGFILES.files = sensorfw.conf
        DBUSCONFIGFILES.path = /etc/dbus-1/system.d
        INSTALLS += DBUSCONFIGFILES

        SENSORDCONFIGFILES.files = config/90-sensord-default.conf
        SENSORDCONFIGFILES.path = /etc/sensorfw/sensord.conf.d
        INSTALLS += SENSORDCONFIGFILES

        SENSORDCONFIGHELPER.files = config/sensord-daemon-conf-setup
        SENSORDCONFIGHELPER.path = /usr/bin
        INSTALLS += SENSORDCONFIGHELPER

        SENSORSYSTEMD.files = rpm/sensord.service
        SENSORSYSTEMD.path = /lib/systemd/system
        INSTALLS += SENSORSYSTEMD
    }
}

equals(QT_MAJOR_VERSION, 4):  {
    OTHER_FILES += rpm/sensorfw.spec \
                   rpm/sensorfw.yaml
}

equals(QT_MAJOR_VERSION, 5):  {
    OTHER_FILES += rpm/sensorfw-qt5.spec \
                   rpm/sensorfw-qt5.yaml
    OTHER_FILES += rpm/sensorfw-qt5-hybris.spec \
                   rpm/sensorfw-qt5-hybris.yaml

}
OTHER_FILES += config/*

