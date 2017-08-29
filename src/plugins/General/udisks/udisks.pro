include(../../plugins.pri)

QT += dbus

TARGET = $$PLUGINS_PREFIX/General/udisks
QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libudisks.so

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/General
INSTALLS += target


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

LIBS += -lqmmpui

FORMS += settingsdialog.ui
