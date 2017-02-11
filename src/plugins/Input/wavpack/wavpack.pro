include(../../plugins.pri)
FORMS +=
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
TARGET = $$PLUGINS_PREFIX/Input/wavpack

CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    LIBS += -lqmmp
    PKGCONFIG += wavpack
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libwavpack.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    LIBS += -lqmmp0 -lwavpack
}
