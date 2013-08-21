TEMPLATE = subdirs

include( ../common-config.pri )
include( ../common-install.pri )

SUBDIRS  = accelerometerchain \
           orientationchain \
           magcalibrationchain \
           compasschain
