include(../../plugins.pri)
FORMS += settingsdialog.ui
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

CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

TARGET = $$PLUGINS_PREFIX/Input/ffmpeg

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libffmpeg.so
    LIBS += -lqmmp
    PKGCONFIG += libavcodec libavformat libavutil
}


win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    LIBS += -lqmmp0 -lavcodec.dll -lavformat.dll -lavutil.dll
}

DEFINES += __STDC_CONSTANT_MACROS

RESOURCES = translations/translations.qrc
