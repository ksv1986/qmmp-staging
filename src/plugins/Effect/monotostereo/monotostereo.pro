include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Effect/monotostereo

HEADERS += monotostereoplugin.h \
           effectmonotostereofactory.h

SOURCES += monotostereoplugin.cpp \
           effectmonotostereofactory.cpp

RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/Effect
    INSTALLS += target
}
