include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/PlayListFormats/m3uplaylistformat

CONFIG += link_qmmpui

HEADERS += m3uplaylistformat.h

SOURCES += m3uplaylistformat.cpp

unix {
    target.path = $$LIB_DIR/qmmp/PlayListFormats
    INSTALLS += target
}
