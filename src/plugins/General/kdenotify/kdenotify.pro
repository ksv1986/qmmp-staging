# -------------------------------------------------
# Project created by QtCreator 2009-10-14T17:55:38
# -------------------------------------------------
include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/kdenotify

QT += dbus

SOURCES += kdenotifyfactory.cpp \
    kdenotify.cpp \
    settingsdialog.cpp

HEADERS += kdenotifyfactory.h \
    kdenotify.h \
    settingsdialog.h

FORMS += settingsdialog.ui

RESOURCES += translations/translations.qrc

LIBS += $$QMMPUI_LIB

target.path = $$PLUGIN_DIR/General
images.files = images/app_icon.png \
               images/empty_cover.png
images.path = $$DATA_DIR/qmmp/images/
INSTALLS += target images
