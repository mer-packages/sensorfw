TEMPLATE = subdirs

include( ../common-config.pri )
# split like this as Sailfish only installs hybris plugins
contains(CONFIG,hybris) {
    SUBDIRS = hybrisaccelerometer
    SUBDIRS += hybrisalsadaptor
    SUBDIRS += hybrisgyroscopeadaptor
    SUBDIRS += hybrismagnetometeradaptor
    SUBDIRS += hybrisproximityadaptor
    SUBDIRS += hybrisorientationadaptor

    } else {

SUBDIRS = alsadaptor \
          alsadaptor-evdev \
          alsadaptor-sysfs \
          alsadaptor-ascii \
          tapadaptor \
          accelerometeradaptor \
          magnetometeradaptor \
          magnetometeradaptor-ascii \
          magnetometeradaptor-evdev \
          magnetometeradaptor-ncdk \
          touchadaptor \
          kbslideradaptor \
          proximityadaptor \
          proximityadaptor-evdev \
          proximityadaptor-ascii \
          mrstaccelerometer \
          gyroscopeadaptor \
          gyroscopeadaptor-evdev
SUDBIRS += oemtabletmagnetometeradaptor
SUBDIRS += pegatronaccelerometeradaptor 
SUBDIRS += oemtabletalsadaptor-ascii
SUBDIRS += oaktrailaccelerometer
SUBDIRS += oemtabletaccelerometer
SUDBIRS += oemtabletgyroscopeadaptor
SUBDIRS += steaccelerometeradaptor
SUBDIRS += mpu6050accelerometer

config_hybris {
    SUBDIRS += hybrisaccelerometer
    SUBDIRS += hybrisalsadaptor
    SUBDIRS += hybrisgyroscopeadaptor
    SUBDIRS += hybrismagnetometeradaptor
    SUBDIRS += hybrisproximityadaptor
    SUBDIRS += hybrisorientationadaptor
 }
}


