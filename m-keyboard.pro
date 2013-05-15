CONFIG += ordered 
TARGET = meego-im-plugins
TEMPLATE = subdirs
SUBDIRS = m-keyboard \
      tests \
      fixture_virtualkeyboard \
      translations \

CONFIG(docs) {
    include (doc/doc.pri)
}

#error-correction 
GCONF_DATA = meego-keyboard.schemas
gconf_data.path = /usr/share/gconf/schemas
gconf_data.files = $$GCONF_DATA

INSTALLS += gconf_data \ 

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

QMAKE_CLEAN += build-stamp configure-stamp

