include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Effect/stereo

HEADERS += stereoplugin.h \
           effectstereofactory.h \
           settingsdialog.h

SOURCES += stereoplugin.cpp \
           effectstereofactory.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/Effect
    INSTALLS += target
}
