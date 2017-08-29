include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/wildmidi

HEADERS += decoderwildmidifactory.h \
    decoder_wildmidi.h \
    wildmidihelper.h \
    settingsdialog.h

SOURCES += decoder_wildmidi.cpp \
    decoderwildmidifactory.cpp \
    wildmidihelper.cpp \
    settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += -L/usr/lib \
    -I/usr/include \
    -lWildMidi

target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
