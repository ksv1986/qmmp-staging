include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/udisks

QT += dbus

HEADERS += udisksfactory.h \
           udisksplugin.h \
           udisksmanager.h \
           udisksdevice.h \
           settingsdialog.h

SOURCES += udisksfactory.cpp \
           udisksplugin.cpp \
           udisksmanager.cpp \
           udisksdevice.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

target.path = $$PLUGIN_DIR/General
INSTALLS += target
