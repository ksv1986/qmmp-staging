include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/shout

HEADERS += outputshoutfactory.h \
    shoutoutput.h \
    shoutclient.h \
    settingsdialog.h

SOURCES += outputshoutfactory.cpp \
    shoutoutput.cpp \
    shoutclient.cpp \
    settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Output
    INSTALLS += target
    PKGCONFIG += ogg vorbis vorbisenc shout soxr
}

