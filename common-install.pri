
#
# Common installation specifications
#
#

# Remove gui dependency from everything
QT -= gui

# Path for headers - remember to add files if they should be installed
equals(QT_MAJOR_VERSION, 4): {
    publicheaders.path = /usr/include/sensord
    PLUGINPATH = /usr/lib/sensord
}

equals(QT_MAJOR_VERSION, 5): {
    publicheaders.path = /usr/include/sensord-qt5
    PLUGINPATH = /usr/lib/sensord-qt5
}

# Path for shared libraries
SHAREDLIBPATH = /usr/lib

INSTALLS += publicheaders
