include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/mpeg

HEADERS += decodermpegfactory.h \
    decoder_mad.h \
    decoder_mpg123.h \
    settingsdialog.h \
    tagextractor.h \
    mpegmetadatamodel.h \
    replaygainreader.h

SOURCES += decoder_mad.cpp \
    decoder_mpg123.cpp \
    decodermpegfactory.cpp \
    settingsdialog.cpp \
    tagextractor.cpp \
    mpegmetadatamodel.cpp \
    replaygainreader.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    LIBS += -lmad
    PKGCONFIG += taglib mad libmpg123
}

win32 {
    LIBS += -lmad -ltag.dll
}
