include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/ffmpeg_legacy

HEADERS += decoderffmpegfactory.h \
    decoder_ffmpeg.h \
    settingsdialog.h \
    ffmpegmetadatamodel.h

SOURCES += decoder_ffmpeg.cpp \
    decoderffmpegfactory.cpp \
    settingsdialog.cpp \
    ffmpegmetadatamodel.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

DEFINES += __STDC_CONSTANT_MACROS

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += libavcodec libavformat libavutil
}

win32 {
    LIBS +=-lavcodec.dll -lavformat.dll -lavutil.dll
}
