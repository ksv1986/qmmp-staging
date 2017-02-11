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
win32:HEADERS += ../../../../src/qmmp/decoder.h \
    ../../../../src/qmmp/statehandler.h
TARGET = $$PLUGINS_PREFIX/Input/cue
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libcue.so
CONFIG += warn_on \
    plugin
TEMPLATE = lib
unix{
   LIBS += -lqmmp -L/usr/lib
}

win32{
   LIBS += -lqmmp0
}

contains(CONFIG, WITH_ENCA){
   CONFIG += link_pkgconfig
   DEFINES += WITH_ENCA
   unix:PKGCONFIG += enca
   win32:LIBS += -lenca.dll

}

RESOURCES = translations/translations.qrc
unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
}
