include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/gnomehotkey

QT += dbus

HEADERS += gnomehotkeyfactory.h \
    mediakeys.h

SOURCES += gnomehotkeyfactory.cpp \
    mediakeys.cpp

RESOURCES = translations/translations.qrc

LIBS += -lqmmpui

target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
