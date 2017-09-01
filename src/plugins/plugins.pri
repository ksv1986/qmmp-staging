include(../../qmmp.pri)
INCLUDEPATH += ../../../qmmp ../../../
QMAKE_LIBDIR += ../../../../lib

win32 {
    QMAKE_LIBDIR += ../../../../bin
    PLUGINS_PREFIX=../../../../../bin/plugins
    LIBS += -lqmmp0
    QMMPUI_LIB = -lqmmpui0
}

unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    PLUGINS_PREFIX=../../../../lib/qmmp
    LIBS += -lqmmp
    QMMPUI_LIB = -lqmmpui
}

CONFIG += warn_on plugin lib thread link_pkgconfig hide_symbols
TEMPLATE = lib
