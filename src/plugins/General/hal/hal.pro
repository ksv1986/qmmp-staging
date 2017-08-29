include(../../plugins.pri)

QT += dbus

TARGET = $$PLUGINS_PREFIX/General/hal
QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libhal.so

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

LIBS += -lqmmpui

FORMS += settingsdialog.ui
