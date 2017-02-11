include(../../plugins.pri)

HEADERS += crossfadeplugin.h \
           effectcrossfadefactory.h \
           settingsdialog.h

SOURCES += crossfadeplugin.cpp \
           effectcrossfadefactory.cpp \
           settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Effect/crossfade

CONFIG += warn_on \
plugin

TEMPLATE = lib

RESOURCES = translations/translations.qrc

FORMS += settingsdialog.ui

unix {
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target

    LIBS += -lqmmp -L/usr/lib -I/usr/include
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libcrossfade.so
}

win32 {
    LIBS += -lqmmp0
}

