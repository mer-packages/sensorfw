TEMPLATE = subdirs
SUBDIRS = sensorfakeopen datafaker

sensordiverter.files = sensordiverter.sh
sensordiverter.path = /usr/bin

testconfig.files = 00-automatic-testing.conf
testconfig.path = /usr/share/sensord-tests

INSTALLS += sensordiverter testconfig
