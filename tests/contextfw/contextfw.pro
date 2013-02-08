TEMPLATE = subdirs

testpackage.files = tests.xml orientation/testorientation.py orientation/testorientation-manual.sh als/testals.py stationary/teststationary.py
testpackage.path = /usr/share/sensorfw-contextfw-tests

INSTALLS += testpackage
