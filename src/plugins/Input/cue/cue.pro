include(../../plugins.pri)
FORMS += settingsdialog.ui
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

TARGET = $$PLUGINS_PREFIX/Input/cue
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libcue.so

contains(CONFIG, WITH_ENCA){
   DEFINES += WITH_ENCA
   unix:PKGCONFIG += enca
   win32:LIBS += -lenca.dll
}

RESOURCES = translations/translations.qrc
unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
}
