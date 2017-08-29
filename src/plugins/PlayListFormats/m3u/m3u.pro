include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/PlayListFormats/m3uplaylistformat

HEADERS += m3uplaylistformat.h

SOURCES += m3uplaylistformat.cpp

LIBS += -lqmmpui

unix {
    target.path = $$LIB_DIR/qmmp/PlayListFormats
    INSTALLS += target
    QMAKE_CLEAN =$$PLUGINS_PREFIX/PlayListFormats/libm3uplaylistformat.so
}
