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

RESOURCES = translations/translations.qrc

unix{
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += libsidplayfp
}

win32 {
    LIBS += -lsidplayfp.dll
}
