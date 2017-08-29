include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/PlayListFormats/xspfplaylistformat

SOURCES += xspfplaylistformat.cpp
HEADERS += xspfplaylistformat.h

LIBS += -lqmmpui

unix {
    target.path = $$LIB_DIR/qmmp/PlayListFormats
    INSTALLS += target

    QMAKE_CLEAN =$$PLUGINS_PREFIX/PlayListFormats/libxsplaylistformat.so

}
