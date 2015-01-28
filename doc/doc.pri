DOC_FOLDERS = doc/html

# Folder(s) must exist to be included in Makefile.
for( folder, DOC_FOLDERS ) {
    system( mkdir -p $$(PWD)/$${folder} )
}

# Documentation build target
doctarget.target = docs
contains(MAKE_DOCS,no): {
doctarget.commands = echo \"*** skip building docs ***\"
} else {
doctarget.commands = INPUT=\"$${_PRO_FILE_PWD_}/adaptors \
                     $${_PRO_FILE_PWD_}/datatypes \
                     $${_PRO_FILE_PWD_}/sensord \
                     $${_PRO_FILE_PWD_}/filters \
                     $${_PRO_FILE_PWD_}/sensors \
                     $${_PRO_FILE_PWD_}/chains \
                     $${_PRO_FILE_PWD_}/include \
                     $${_PRO_FILE_PWD_}/qt-api \
                     $${_PRO_FILE_PWD_}/doc/mainpage.h\" \
                     STRIP_FROM_PATH=\"$${_PRO_FILE_PWD_}\" \
                     doxygen $$system(pwd)/Doxyfile
}
QMAKE_EXTRA_TARGETS += doctarget

# Add documentation cleaning to clean target
doccleantarget.target = clean
for( folder, DOC_FOLDERS ) {
    doccleantarget.commands += rm -rf $${folder}
}

QMAKE_EXTRA_TARGETS += doccleantarget

# Documentation install target
documentation.path = /usr/share/doc/sensord-qt5
documentation.files = doc/html
documentation.depends = docs

INSTALLS += documentation
