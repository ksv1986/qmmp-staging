include(../../plugins.pri)

HEADERS += stereoplugin.h \
           effectstereofactory.h \
           settingsdialog.h

SOURCES += stereoplugin.cpp \
           effectstereofactory.cpp \
           settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Effect/stereo

CONFIG += warn_on \
plugin

TEMPLATE = lib

RESOURCES = translations/translations.qrc

FORMS += settingsdialog.ui

unix {
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target

    LIBS += -lqmmp -L/usr/lib -I/usr/include
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libstereo.so
}

win32 {
    LIBS += -lqmmp0
}
