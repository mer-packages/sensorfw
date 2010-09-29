TEMPLATE = subdirs

include( ../common-config.pri )

internal {
    SUBDIRS = alsadaptor \
              alsadaptor-sysfs \
              tapadaptor \
              accelerometeradaptor \
              magnetometeradaptor \
              magnetometeradaptor-ascii \
              touchadaptor \
              kbslideradaptor \
              proximityadaptor \
              proximityadaptor-evdev \
              proximityadaptor-ascii \
              mrstaccelerometer
}
