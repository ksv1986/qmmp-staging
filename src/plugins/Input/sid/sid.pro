include(../../plugins.pri)

HEADERS += decodersidfactory.h \
    decoder_sid.h \
    sidhelper.h \
    settingsdialog.h
SOURCES += decoder_sid.cpp \
    decodersidfactory.cpp \
    sidhelper.cpp \
    settingsdialog.cpp
FORMS += \
    settingsdialog.ui


TARGET = $$PLUGINS_PREFIX/Input/sid
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libsid.so
CONFIG += warn_on \
          plugin \
          link_pkgconfig
TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix{
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    LIBS += -lqmmp
    PKGCONFIG += libsidplayfp
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    LIBS += -lqmmp0 -lsidplayfp.dll
}
