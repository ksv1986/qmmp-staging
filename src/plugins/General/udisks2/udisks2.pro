include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/udisks2

QT += dbus

HEADERS += udisks2factory.h \
           udisks2plugin.h \
           udisks2manager.h \
           udisks2device.h \
           settingsdialog.h

SOURCES += udisks2factory.cpp \
           udisks2plugin.cpp \
           udisks2manager.cpp \
           udisks2device.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
