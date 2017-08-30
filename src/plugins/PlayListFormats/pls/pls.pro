include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/PlayListFormats/plsplaylistformat

CONFIG += link_qmmpui

SOURCES += plsplaylistformat.cpp

HEADERS += plsplaylistformat.h

unix {
    target.path = $$LIB_DIR/qmmp/PlayListFormats
    INSTALLS += target
}
