include(../../plugins.pri)

HEADERS += soxresampler.h \
 effectsoxrfactory.h \
 settingsdialog.h

SOURCES += soxresampler.cpp \
 effectsoxrfactory.cpp \
 settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Effect/soxr
QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libsoxr.so
INCLUDEPATH += ../../../
CONFIG += warn_on \
plugin \
link_pkgconfig

PKGCONFIG += soxr
TEMPLATE = lib
QMAKE_LIBDIR += ../../../../lib
LIBS += -lqmmp -L/usr/lib -I/usr/include

RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Effect
INSTALLS += target

FORMS += settingsdialog.ui

