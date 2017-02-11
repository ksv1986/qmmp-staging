include(../../plugins.pri)

CONFIG += warn_on \
plugin  \
 lib

TARGET =$$PLUGINS_PREFIX/PlayListFormats/xspfplaylistformat

TEMPLATE = lib

SOURCES += xspfplaylistformat.cpp
HEADERS += xspfplaylistformat.h

unix {
    target.path = $$LIB_DIR/qmmp/PlayListFormats
    INSTALLS += target

    QMAKE_CLEAN =$$PLUGINS_PREFIX/PlayListFormats/libxsplaylistformat.so
    LIBS += -lqmmpui -lqmmp
}

win32 {
    LIBS += -lqmmpui0 -lqmmp0
}

