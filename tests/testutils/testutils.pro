TEMPLATE = subdirs
SUBDIRS = sensorfakeopen datafaker

sensordiverter.files = sensordiverter.sh
sensordiverter.path = /usr/bin

testconfig.files = 00-automatic-testing-rm680.conf 00-automatic-testing-rm696.conf
testconfig.path = /usr/share/sensorfw-tests

INSTALLS += sensordiverter testconfig
