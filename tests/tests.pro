CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = filters\
          adaptors \
          chains \
          client \
          testapp \
          dataflow \
          benchmark \
          testutils \
          deadclient \
          metadata 

#disabled tests due to requirement of mcetool
contains(CONFIG,mce) {
    SUBDIRS += powermanagement
}
contextprovider {
    SUBDIRS += contextfw
}
testdefinition.files = tests.xml
testdefinition.path = /usr/share/sensorfw-tests

INSTALLS += testdefinition
