
#
# Common configuration options
#
#

QT += dbus
QT -= gui
CONFIG += debug
CONFIG += thread

# Maemo specific options
#CONFIG += contextprovider

# DEPRECATED: CompassChain is now provided by separate package
#             sensorfw-plugins-compasslogic
### Note: compass now works for x86 and armel both. debian/rules takes care
### of figuring out which platform binary to pick. If building manually, the
### default is armel. If internal sensors are included in manual build for x86,
### uncomment the following line:
#### CONFIG+= x86build

profile {
  QMAKE_CXXFLAGS += -pg
  QMAKE_LFLAGS += -pg
}

profile-libc {
  QMAKE_LFLAGS += -lc_p
}
