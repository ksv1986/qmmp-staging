include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/opus

HEADERS += decoderopusfactory.h \
    decoder_opus.h \
    opusmetadatamodel.h \
    replaygainreader.h

SOURCES += decoder_opus.cpp \
    decoderopusfactory.cpp \
    opusmetadatamodel.cpp \
    replaygainreader.cpp

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += taglib opus opusfile
}

win32 {
    LIBS += -lopusfile -lopus -ltag.dll -lm
}
