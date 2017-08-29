include(../../plugins.pri)
FORMS +=
HEADERS += decoderopusfactory.h \
    decoder_opus.h \
    opusmetadatamodel.h \
    replaygainreader.h \
    opusproperties.h \
    opusfile.h \
    tdebug.h
SOURCES += decoder_opus.cpp \
    decoderopusfactory.cpp \
    opusmetadatamodel.cpp \
    replaygainreader.cpp \
    opusproperties.cpp \
    opusfile.cpp \
    tdebug.cpp
TARGET = $$PLUGINS_PREFIX/Input/opus


RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    PKGCONFIG += taglib opus opusfile
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libopus.so
}

win32 {
    LIBS += -lopusfile -lopus -ltag.dll -lm
}
