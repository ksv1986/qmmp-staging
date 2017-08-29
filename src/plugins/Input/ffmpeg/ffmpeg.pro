include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/ffmpeg

HEADERS += decoderffmpegfactory.h \
    decoder_ffmpeg.h \
    settingsdialog.h \
    ffmpegmetadatamodel.h \
    replaygainreader.h

SOURCES += decoder_ffmpeg.cpp \
    decoderffmpegfactory.cpp \
    settingsdialog.cpp \
    ffmpegmetadatamodel.cpp \
    replaygainreader.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

DEFINES += __STDC_CONSTANT_MACROS

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += libavcodec libavformat libavutil
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    LIBS += -lavcodec.dll -lavformat.dll -lavutil.dll
}
