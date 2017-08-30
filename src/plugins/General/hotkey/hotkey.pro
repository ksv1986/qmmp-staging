include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/hotkey

CONFIG += link_qmmpui

HEADERS += hotkeyfactory.h \
           hotkeymanager.h \
           settingsdialog.h \
           hotkeydialog.h

SOURCES += hotkeyfactory.cpp \
           settingsdialog.cpp \
           hotkeydialog.cpp \
           hotkeymanager_x11.cpp \
           hotkeymanager_win.cpp

FORMS += settingsdialog.ui \
         hotkeydialog.ui

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
    PKGCONFIG += x11
    DEFINES += HAVE_XKBLIB_H
}
