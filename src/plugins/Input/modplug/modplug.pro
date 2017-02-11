include(../../plugins.pri)
FORMS += settingsdialog.ui
HEADERS += decodermodplugfactory.h \
    decoder_modplug.h \
    settingsdialog.h \
    archivereader.h \
    modplugmetadatamodel.h
SOURCES += decoder_modplug.cpp \
    decodermodplugfactory.cpp \
    settingsdialog.cpp \
    archivereader.cpp \
    modplugmetadatamodel.cpp
TARGET = $$PLUGINS_PREFIX/Input/modplug

DEFINES += HAVE_STDINT_H \
    HAVE_INTTYPES_H
CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    LIBS += -lqmmp
    PKGCONFIG += libmodplug
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libmodplug.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    LIBS += -lqmmp0 -lmodplug
    DEFINES -= UNICODE
}
