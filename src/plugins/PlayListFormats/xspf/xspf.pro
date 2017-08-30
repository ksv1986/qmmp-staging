include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/PlayListFormats/xspfplaylistformat
CONFIG += link_qmmpui

SOURCES += xspfplaylistformat.cpp

HEADERS += xspfplaylistformat.h

unix {
    target.path = $$LIB_DIR/qmmp/PlayListFormats
    INSTALLS += target
}
