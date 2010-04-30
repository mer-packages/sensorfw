TEMPLATE = subdirs

include( ../common-config.pri )
include( ../common-install.pri )

internal {
    SUBDIRS  = accelerometerchain \
               orientationchain
}




