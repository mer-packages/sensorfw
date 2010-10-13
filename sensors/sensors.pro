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
           magnetometersensor

ariane {
    SUBDIRS += arianeplugin \
               filteredplugin
}

motionband:SUBDIRS += raworientationplugin

contextprovider:SUBDIRS += contextplugin
