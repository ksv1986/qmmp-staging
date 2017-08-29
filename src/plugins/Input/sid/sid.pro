include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/sid

HEADERS += decodersidfactory.h \
    decoder_sid.h \
    sidhelper.h \
    settingsdialog.h

SOURCES += decoder_sid.cpp \
    decodersidfactory.cpp \
    sidhelper.cpp \
    settingsdialog.cpp

RESOURCES = translations/translations.qrc

FORMS += settingsdialog.ui

unix{
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += libsidplayfp
}

win32 {
    LIBS += -lsidplayfp.dll
}
