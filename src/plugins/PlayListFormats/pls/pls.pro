include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/PlayListFormats/plsplaylistformat

SOURCES += plsplaylistformat.cpp

HEADERS += plsplaylistformat.h

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$LIB_DIR/qmmp/PlayListFormats
    INSTALLS += target
}
