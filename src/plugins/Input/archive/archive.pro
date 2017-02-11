include(../../plugins.pri)

HEADERS += \
    archiveinputdevice.h \
    decoderarchivefactory.h \
    decoder_archive.h \
    archivetagreader.h \
    archivemetadatamodel.h

SOURCES += \
    archiveinputdevice.cpp \
    decoderarchivefactory.cpp \
    decoder_archive.cpp \
    archivetagreader.cpp \
    archivemetadatamodel.cpp

TARGET=$$PLUGINS_PREFIX/Input/archive

CONFIG += warn_on \
plugin \
link_pkgconfig

TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    LIBS += -lqmmp
    PKGCONFIG += libarchive taglib
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libarc.so
}

win32 {
    LIBS += -lqmmp0 -larchive -ltag
}
