include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/mpeg

HEADERS += decodermpegfactory.h \
    settingsdialog.h \
    tagextractor.h \
    mpegmetadatamodel.h

SOURCES += decodermpegfactory.cpp \
    settingsdialog.cpp \
    tagextractor.cpp \
    mpegmetadatamodel.cpp

contains(CONFIG, WITH_MAD){
    HEADERS += decoder_mad.h
    SOURCES += decoder_mad.cpp
    DEFINES += WITH_MAD
}

contains(CONFIG, WITH_MPG123){
    HEADERS += decoder_mpg123.h
    SOURCES += decoder_mpg123.cpp
    DEFINES += WITH_MPG123
}

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    PKGCONFIG += taglib
    contains(CONFIG, WITH_MAD):PKGCONFIG += mad
    contains(CONFIG, WITH_MPG123):PKGCONFIG += libmpg123
}

win32 {
    LIBS += -ltag.dll
    contains(CONFIG, WITH_MAD):LIBS += -lmad
    contains(CONFIG, WITH_MPG123):LIBS += -lmpg123.dll
}
