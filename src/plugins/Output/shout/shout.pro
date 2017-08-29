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


RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Output
    INSTALLS += target
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libshout.so
    PKGCONFIG += ogg vorbis vorbisenc shout soxr
}

FORMS += \
    settingsdialog.ui
