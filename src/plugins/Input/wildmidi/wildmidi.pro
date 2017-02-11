include(../../plugins.pri)
FORMS += settingsdialog.ui
HEADERS += decoderwildmidifactory.h \
    decoder_wildmidi.h \
    wildmidihelper.h \
    settingsdialog.h
SOURCES += decoder_wildmidi.cpp \
    decoderwildmidifactory.cpp \
    wildmidihelper.cpp \
    settingsdialog.cpp
TARGET = $$PLUGINS_PREFIX/Input/wildmidi
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libwildmidi.so
CONFIG += warn_on \
    plugin
TEMPLATE = lib
LIBS += -lqmmp \
    -L/usr/lib \
    -I/usr/include \
    -lWildMidi

RESOURCES = translations/translations.qrc
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
