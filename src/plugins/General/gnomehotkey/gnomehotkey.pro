include(../../plugins.pri)

CONFIG += warn_on \
plugin  \
 lib \
 qdbus

TARGET = $$PLUGINS_PREFIX/General/gnomehotkey
QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libgnomehotkey.so

TEMPLATE = lib

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/General
INSTALLS += target


HEADERS += gnomehotkeyfactory.h \
    mediakeys.h

SOURCES += gnomehotkeyfactory.cpp \
    mediakeys.cpp

LIBS += -lqmmpui -lqmmp
