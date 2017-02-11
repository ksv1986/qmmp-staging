include(../../plugins.pri)

HEADERS += outputnullfactory.h \
           outputnull.h

SOURCES += outputnullfactory.cpp \
           outputnull.cpp


TARGET=$$PLUGINS_PREFIX/Output/null

CONFIG += warn_on \
thread \
plugin

TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Output
    INSTALLS += target
    LIBS += -lqmmp
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libnull.so
}

win32 {
    LIBS += -lqmmp0
}
