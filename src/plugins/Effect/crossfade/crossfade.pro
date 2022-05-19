include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Effect/crossfade

HEADERS += crossfadeplugin.h \
           effectcrossfadefactory.h \
           settingsdialog.h

SOURCES += crossfadeplugin.cpp \
           effectcrossfadefactory.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/Effect
    INSTALLS += target
}
