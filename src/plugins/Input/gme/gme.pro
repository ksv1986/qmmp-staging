include(../../plugins.pri)

HEADERS += decodergmefactory.h \
    decoder_gme.h \
    gmehelper.h \
    settingsdialog.h
SOURCES += decoder_gme.cpp \
    decodergmefactory.cpp \
    gmehelper.cpp \
    settingsdialog.cpp

FORMS += settingsdialog.ui

TARGET = $$PLUGINS_PREFIX/Input/gme
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libgme.so

RESOURCES = translations/translations.qrc

LIBS += -lqmmp

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
