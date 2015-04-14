TEMPLATE = subdirs
SUBDIRS = coordinatealignfilter \
          declinationfilter \
          orientationinterpreter \
          rotationfilter \
          downsamplefilter \
          avgaccfilter \
          magcoordinatealignfilter

include(../common-install.pri)
publicheaders.files = *.h
publicheaders.path = $${publicheaders.path}/filters

