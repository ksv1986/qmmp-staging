include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/vorbis

HEADERS += decodervorbisfactory.h \
    decoder_vorbis.h \
    vorbismetadatamodel.h

SOURCES += decoder_vorbis.cpp \
    decodervorbisfactory.cpp \
    vorbismetadatamodel.cpp

RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    PKGCONFIG += taglib ogg vorbisfile vorbis
}

win32 {
    LIBS += -lvorbisfile -lvorbis -logg -ltag.dll -lm
    LD_FLAGS += -no-undefined
}
