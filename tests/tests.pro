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
          powermanagement \
          metadata \
          external
contextprovider {
    SUBDIRS += contextfw
}
testdefinition.files = tests.xml
testdefinition.path = /usr/share/sensorfw-tests

INSTALLS += testdefinition
