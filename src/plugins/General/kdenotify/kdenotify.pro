# -------------------------------------------------
# Project created by QtCreator 2009-10-14T17:55:38
# -------------------------------------------------
include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/kdenotify

QT += dbus

DEFINES += APP_NAME_SUFFIX=\\\"$$APP_NAME_SUFFIX\\\"

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
images.path = $$DATA_DIR/qmmp$${APP_NAME_SUFFIX}/images/
INSTALLS += target images
