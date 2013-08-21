TEMPLATE = subdirs
SUBDIRS = coordinatealignfilter \
          declinationfilter \
          orientationinterpreter \
          rotationfilter \
          downsamplefilter \
          avgaccfilter

include(../common-install.pri)
publicheaders.files = *.h
publicheaders.path = $${publicheaders.path}/filters

