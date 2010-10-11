TEMPLATE = subdirs

include( ../common-config.pri )

SUBDIRS = alsadaptor \
          alsadaptor-sysfs \
          alsadaptor-ascii \
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
