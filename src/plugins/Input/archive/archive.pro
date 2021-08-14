include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/archive

HEADERS += \
    archiveinputdevice.h \
    decoderarchivefactory.h \
    decoder_archive.h \
    archivetagreader.h

SOURCES += \
    archiveinputdevice.cpp \
    decoderarchivefactory.cpp \
    decoder_archive.cpp \
    archivetagreader.cpp

RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    PKGCONFIG += libarchive taglib
}
