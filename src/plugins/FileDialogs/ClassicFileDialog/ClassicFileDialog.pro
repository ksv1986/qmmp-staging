include(../../plugins.pri)

INCLUDEPATH += ../../../../src

TARGET =$$PLUGINS_PREFIX/FileDialogs/classicfiledialog


HEADERS += classicfiledialog.h \
           classicfiledialogimpl.h

SOURCES += classicfiledialog.cpp \
           classicfiledialogimpl.cpp


FORMS += classicfiledialog.ui

CONFIG += warn_on plugin

TEMPLATE = lib

QMAKE_LIBDIR += ../../../../lib

#RESOURCES += translations/translations.qrc

unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    target.path = $$LIB_DIR/classic/FileDialogs
    INSTALLS += target

    QMAKE_CLEAN =$$PLUGINS_PREFIX/FileDialogs/libclassicfiledialog.so
    LIBS += -lqmmpui -lqmmp
}

win32 {
    LIBS += -lqmmpui0 -lqmmp0
    QMAKE_LIBDIR += ../../../../bin
}
