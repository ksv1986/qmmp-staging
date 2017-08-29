include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/flac

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

RESOURCES = translations/translations.qrc

LIBS += -lqmmp

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += taglib flac
}

win32 {
    LIBS += -lflac -logg -ltag.dll -lm
}
