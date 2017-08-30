include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/hal

QT += dbus
CONFIG += link_qmmpui

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

FORMS += settingsdialog.ui


RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
