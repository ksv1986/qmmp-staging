include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/cue

HEADERS += decodercuefactory.h \
    cueparser.h \
    decoder_cue.h \
    settingsdialog.h \
    cuemetadatamodel.h

SOURCES += decoder_cue.cpp \
    decodercuefactory.cpp \
    cueparser.cpp \
    settingsdialog.cpp \
    cuemetadatamodel.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

contains(CONFIG, WITH_ENCA) {
    DEFINES += WITH_ENCA
    unix:PKGCONFIG += enca
    win32:LIBS += -lenca.dll
}
unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
}
