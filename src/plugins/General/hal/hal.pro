include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/hal

QT += dbus

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

LIBS += $$QMMPUI_LIB

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
