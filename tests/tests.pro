CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = filters\
          adaptors \
          chains \
          contextfw \
          client \
          createsensor \
          dataflow \
          benchmark \
          testutils \
          deadclient \
          powermanagement \
          metadata \
          external

testdefinition.files = tests.xml
testdefinition.path = /usr/share/sensord-tests

INSTALLS += testdefinition
