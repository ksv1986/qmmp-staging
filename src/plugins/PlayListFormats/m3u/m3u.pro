include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/PlayListFormats/m3uplaylistformat

HEADERS += m3uplaylistformat.h

SOURCES += m3uplaylistformat.cpp

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/PlayListFormats
    INSTALLS += target
}
