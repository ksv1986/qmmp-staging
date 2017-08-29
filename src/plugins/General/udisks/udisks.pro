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

LIBS += -lqmmpui

target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
