QMAKE_EXTRA_TARGETS += check
check.target = check
check.commands = LD_LIBRARY_PATH=../../m-keyboard/:$(LD_LIBRARY_PATH) ./$$TARGET
check.depends += $$TARGET

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.commands = ../rt.sh $$TARGET
check-xml.depends += $$TARGET

QMAKE_CLEAN += *.log *~

target.path = /usr/lib/meego-keyboard-tests/$$TARGET
INSTALLS += target

QMAKE_CXXFLAGS += -Werror

DEFINES += UNIT_TEST

QT += testlib

INCLUDEPATH += \
    $$WIDGETS_DIR \
    $$COMMON_DIR \
    $$MKEYBOARD_DIR \
    $$UTILS_DIR \

HEADERS += \
    $$UTILS_DIR/utils.h \

SOURCES += \
    $$UTILS_DIR/utils.cpp \

LIBS += \
    -L $$MKEYBOARD_DIR \

# coverage flags are off per default, but can be turned on via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += *.gcno *.gcda

LIBS += -lmeegoimengine
CONFIG += meegoimframework meegoimengine

OBJECTS_DIR = .obj
MOC_DIR = .moc

meegotouch {
    DEFINES += HAVE_MEEGOTOUCH
}

