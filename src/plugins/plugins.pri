include(../../qmmp.pri)
INCLUDEPATH += ../../../qmmp ../../../
QMAKE_LIBDIR += ../../../../lib

win32 {
    QMAKE_LIBDIR += ../../../../bin
    PLUGINS_PREFIX=../../../../../bin/plugins
    LIBS += -lqmmp0
}

unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    PLUGINS_PREFIX=../../../../lib/qmmp
    LIBS += -lqmmp
}

CONFIG += warn_on plugin lib thread link_pkgconfig
TEMPLATE = lib

contains(CONFIG, link_qmmpui) {
    unix:LIBS += -lqmmpui
    win32:LIBS += -lqmmpui0
}
