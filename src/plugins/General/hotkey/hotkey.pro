include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/hotkey

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

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
    PKGCONFIG += x11
}
