include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/vorbis

HEADERS += decodervorbisfactory.h \
    decoder_vorbis.h \
    vorbismetadatamodel.h \
    replaygainreader.h

SOURCES += decoder_vorbis.cpp \
    decodervorbisfactory.cpp \
    vorbismetadatamodel.cpp \
    replaygainreader.cpp

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += taglib ogg vorbisfile vorbis
}

win32 {
    LIBS += -lvorbisfile -lvorbis -logg -ltag.dll -lm
    LD_FLAGS += -no-undefined
}
