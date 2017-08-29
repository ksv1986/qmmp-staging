include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/modplug

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

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

DEFINES += HAVE_STDINT_H \
    HAVE_INTTYPES_H

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += libmodplug
}

win32 {
    LIBS += -lmodplug
    DEFINES -= UNICODE
}
