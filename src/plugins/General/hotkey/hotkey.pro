include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/hotkey

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/General

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

unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
    QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libhotkey.so
    PKGCONFIG += x11
    DEFINES += HAVE_XKBLIB_H
}


LIBS += -lqmmpui

