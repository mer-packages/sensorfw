CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = filters\
          adaptors \
          chains \
          contextfw \
          client \
          dataflow \
          benchmark \
          testutils \
          deadclient \
          powermanagement \
          external

testdefinition.files = tests.xml
testdefinition.path = /usr/share/sensord-tests

INSTALLS += testdefinition
