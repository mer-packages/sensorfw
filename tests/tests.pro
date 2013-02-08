CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = filters\
          adaptors \
          chains \
          contextfw \
          client \
          testapp \
          dataflow \
          benchmark \
          testutils \
          deadclient \
          powermanagement \
          metadata \
          external

testdefinition.files = tests.xml
testdefinition.path = /usr/share/sensorfw-tests

INSTALLS += testdefinition
