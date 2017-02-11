include(../../plugins.pri)
FORMS +=
HEADERS += decodervorbisfactory.h \
    decoder_vorbis.h \
    vorbismetadatamodel.h \
    replaygainreader.h
SOURCES += decoder_vorbis.cpp \
    decodervorbisfactory.cpp \
    vorbismetadatamodel.cpp \
    replaygainreader.cpp
TARGET = $$PLUGINS_PREFIX/Input/vorbis

CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    PKGCONFIG += taglib ogg vorbisfile vorbis
    LIBS += -lqmmp
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libvorbis.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    LIBS += -lqmmp0 -lvorbisfile -lvorbis -logg -ltag.dll -lm
    LD_FLAGS += -no-undefined
}
