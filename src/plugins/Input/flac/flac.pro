include(../../plugins.pri)
FORMS +=
HEADERS += decoderflacfactory.h \
    decoder_flac.h \
    cueparser.h \
    flacmetadatamodel.h \
    replaygainreader.h
SOURCES += decoder_flac.cpp \
    decoderflacfactory.cpp \
    cueparser.cpp \
    flacmetadatamodel.cpp \
    replaygainreader.cpp
TARGET = $$PLUGINS_PREFIX/Input/flac

CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += taglib flac
    LIBS += -lqmmp
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libflac.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    LIBS += -lqmmp0 -lflac -logg -ltag.dll -lm
}
