include(../../qmmp.pri)
INCLUDEPATH += ../../../qmmp ../../../
QMAKE_LIBDIR += ../../../../lib

win32 {
    QMAKE_LIBDIR += ../../../../bin
    PLUGINS_PREFIX=../../../../../bin/plugins
    LIBS += -lqmmp1
    QMMPUI_LIB = -lqmmpui1
}

unix {
    PLUGINS_PREFIX=../../../../lib/qmmp-$${QMMP_VERSION_MAJOR}.$${QMMP_VERSION_MINOR}
    LIBS += -lqmmp$$APP_NAME_SUFFIX
    QMMPUI_LIB = -lqmmpui$$APP_NAME_SUFFIX
}


CONFIG += warn_on plugin lib thread link_pkgconfig
TEMPLATE = lib
