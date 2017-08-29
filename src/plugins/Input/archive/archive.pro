include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/archive

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

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += libarchive taglib
}
