include(../../plugins.pri)

HEADERS += outputshoutfactory.h \
    shoutoutput.h \
    shoutclient.h \
    settingsdialog.h

SOURCES += outputshoutfactory.cpp \
    shoutoutput.cpp \
    shoutclient.cpp \
    settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Output/shout

INCLUDEPATH += ../../../
QMAKE_LIBDIR += ../../../../lib

CONFIG += warn_on \
thread \
plugin \
link_pkgconfig

TEMPLATE = lib

#RESOURCES = translations/translations.qrc

unix {
    isEmpty (LIB_DIR){
    LIB_DIR = /lib
    }

    target.path = $$LIB_DIR/qmmp/Output
    INSTALLS += target
    LIBS += -lqmmp
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libshout.so
    PKGCONFIG += ogg vorbis vorbisenc shout soxr
}

win32 {
    LIBS += -lqmmp0
    QMAKE_LIBDIR += ../../../../bin
}

FORMS += \
    settingsdialog.ui
