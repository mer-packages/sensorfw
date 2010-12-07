TEMPLATE = subdirs

include( ../common-config.pri )
include( ../common-install.pri )

SUBDIRS  = accelerometersensor \
           orientationsensor \
           tapsensor \
           alssensor \
           proximitysensor \
           compasssensor \
           rotationsensor \
           magnetometersensor \
           gyroscopesensor

contextprovider:SUBDIRS += contextplugin
