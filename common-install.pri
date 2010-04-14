
#
# Common installation specifications
#
#

# Remove gui dependency from everything
QT -= gui

# Path for headers - remember to add files if they should be installed
publicheaders.path = /usr/include/sensord

# Path for shared libraries
SHAREDLIBPATH = /usr/lib
PLUGINPATH = /usr/lib/sensord

INSTALLS += publicheaders
