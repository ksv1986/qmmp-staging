include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/gme

HEADERS += decodergmefactory.h \
    decoder_gme.h \
    gmehelper.h \
    settingsdialog.h

SOURCES += decoder_gme.cpp \
    decodergmefactory.cpp \
    gmehelper.cpp \
    settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

unix{
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    LIBS += -L/usr/lib \
      -L/usr/local/lib \
      -I/usr/include \
      -I/usr/local/include \
      -lgme
}

win32 {
    LIBS += -lgme.dll
}
