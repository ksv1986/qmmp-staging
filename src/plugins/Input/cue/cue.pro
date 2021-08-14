include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/cue

HEADERS += decodercuefactory.h \
    decoder_cue.h \
    settingsdialog.h \
    cuemetadatamodel.h \
    cuefile.h

SOURCES += decoder_cue.cpp \
    decodercuefactory.cpp \
    settingsdialog.cpp \
    cuemetadatamodel.cpp \
    cuefile.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

contains(CONFIG, WITH_ENCA) {
    DEFINES += WITH_ENCA
    unix:PKGCONFIG += enca
    win32:LIBS += -lenca.dll
}
unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
}
