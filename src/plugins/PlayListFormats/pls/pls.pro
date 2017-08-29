include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/PlayListFormats/plsplaylistformat

SOURCES += plsplaylistformat.cpp

HEADERS += plsplaylistformat.h

LIBS += -lqmmpui

unix {
    target.path = $$LIB_DIR/qmmp/PlayListFormats
    INSTALLS += target
    QMAKE_CLEAN =$$PLUGINS_PREFIX/PlayListFormats/plsplaylistformat.so
}
