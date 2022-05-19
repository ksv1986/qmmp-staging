include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/fileops

HEADERS += fileopsfactory.h \
           fileops.h \
           settingsdialog.h \
           hotkeydialog.h

SOURCES += fileopsfactory.cpp \
           fileops.cpp \
           settingsdialog.cpp \
           hotkeydialog.cpp

FORMS += settingsdialog.ui \
         hotkeydialog.ui

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
