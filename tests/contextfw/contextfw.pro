TEMPLATE = subdirs

# Not used by anything anymroe
#SUBDIRS = fakeopen

testpackage.files = tests.xml orientation/testorientation.py orientation/testorientation-manual.sh als/testals.py
testpackage.path = /usr/share/sensord-contextfw-tests

INSTALLS += testpackage
