include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/ffmpeg

HEADERS += decoderffmpegfactory.h \
    decoder_ffmpeg.h \
    settingsdialog.h \
    ffmpegmetadatamodel.h \
    replaygainreader.h \
    decoder_ffmpegcue.h

SOURCES += decoder_ffmpeg.cpp \
    decoderffmpegfactory.cpp \
    settingsdialog.cpp \
    ffmpegmetadatamodel.cpp \
    replaygainreader.cpp \
    decoder_ffmpegcue.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

DEFINES += __STDC_CONSTANT_MACROS

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    PKGCONFIG += libavcodec libavformat libavutil
}

win32 {
    LIBS += -lavcodec.dll -lavformat.dll -lavutil.dll
}
