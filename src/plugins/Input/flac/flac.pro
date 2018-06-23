include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/flac

HEADERS += decoderflacfactory.h \
    decoder_flac.h \
    cueparser.h \
    flacmetadatamodel.h

SOURCES += decoder_flac.cpp \
    decoderflacfactory.cpp \
    cueparser.cpp \
    flacmetadatamodel.cpp

RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    PKGCONFIG += taglib flac
}

win32 {
    LIBS += -lflac -logg -ltag.dll -lm
}
