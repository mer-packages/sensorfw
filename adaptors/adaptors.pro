TEMPLATE = subdirs

include( ../common-config.pri )

internal {
    SUBDIRS = alsadaptor \
              tapadaptor \
              accelerometeradaptor \
              magnetometeradaptor \
              touchadaptor \
              kbslideradaptor \
              proximityadaptor
}

motionband {
     SUBDIRS += mbadaptor
}

ariane {
     SUBDIRS += arianeadaptor
}
