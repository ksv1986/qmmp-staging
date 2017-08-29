include(../../plugins.pri)

HEADERS += crossfadeplugin.h \
           effectcrossfadefactory.h \
           settingsdialog.h

SOURCES += crossfadeplugin.cpp \
           effectcrossfadefactory.cpp \
           settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Effect/crossfade

RESOURCES = translations/translations.qrc

FORMS += settingsdialog.ui

unix {
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libcrossfade.so
}


