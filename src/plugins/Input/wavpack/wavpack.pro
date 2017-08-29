include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/wavpack

HEADERS += decoderwavpackfactory.h \
    decoder_wavpack.h \
    cueparser.h \
    wavpackmetadatamodel.h \
    replaygainreader.h

SOURCES += decoder_wavpack.cpp \
    decoderwavpackfactory.cpp \
    cueparser.cpp \
    wavpackmetadatamodel.cpp \
    replaygainreader.cpp


RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += wavpack
}

win32 {
    LIBS += -lwavpack
}
