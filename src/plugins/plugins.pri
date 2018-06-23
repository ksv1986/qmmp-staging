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
    PLUGINS_PREFIX=../../../../lib/qmmp-$${QMMP_VERSION_MAJOR}.$${QMMP_VERSION_MINOR}
    LIBS += -lqmmp
    QMMPUI_LIB = -lqmmpui
}


CONFIG += warn_on plugin lib thread link_pkgconfig
TEMPLATE = lib
