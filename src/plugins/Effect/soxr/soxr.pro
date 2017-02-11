include(../../plugins.pri)

HEADERS += soxresampler.h \
 effectsoxrfactory.h \
 settingsdialog.h

SOURCES += soxresampler.cpp \
 effectsoxrfactory.cpp \
 settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Effect/soxr
QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libsoxr.so
CONFIG += warn_on \
plugin

TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target
    CONFIG += link_pkgconfig
    PKGCONFIG += soxr
    LIBS += -lqmmp -L/usr/lib -I/usr/include
}

win32 {
    LIBS += -lqmmp0 -lsoxr
}

FORMS += settingsdialog.ui

