include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/PlayListFormats/xspfplaylistformat

SOURCES += xspfplaylistformat.cpp

HEADERS += xspfplaylistformat.h

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/PlayListFormats
    INSTALLS += target
}
