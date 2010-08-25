TEMPLATE = subdirs

include( ../common-config.pri )

internal {
    SUBDIRS = alsadaptor \
              alsadaptor-sysfs \
              tapadaptor \
              accelerometeradaptor \
              magnetometeradaptor \
              touchadaptor \
              kbslideradaptor \
              proximityadaptor \
              mrstaccelerometer
}
