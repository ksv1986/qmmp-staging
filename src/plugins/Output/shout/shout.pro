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

CONFIG += warn_on \
thread \
plugin \
link_pkgconfig

TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Output
    INSTALLS += target
    LIBS += -lqmmp
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libshout.so
    PKGCONFIG += ogg vorbis vorbisenc shout soxr
}

win32 {
    LIBS += -lqmmp0
}

FORMS += \
    settingsdialog.ui
