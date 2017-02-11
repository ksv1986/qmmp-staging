include(../../plugins.pri)

CONFIG += warn_on \
plugin  \
 lib \
 qdbus

TARGET = $$PLUGINS_PREFIX/General/hal
QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libhal.so

TEMPLATE = lib

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/General
INSTALLS += target


HEADERS += halfactory.h \
           halplugin.h \
           halmanager.h \
           haldevice.h \
           settingsdialog.h

SOURCES += halfactory.cpp \
           halplugin.cpp \
           halmanager.cpp \
           haldevice.cpp \
           settingsdialog.cpp

LIBS += -lqmmpui -lqmmp

FORMS += settingsdialog.ui
