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

LIBS += -lqmmp

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    PKGCONFIG += libarchive taglib
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libarc.so
}
