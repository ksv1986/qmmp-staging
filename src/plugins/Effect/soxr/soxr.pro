include(../../plugins.pri)

HEADERS += soxresampler.h \
 effectsoxrfactory.h \
 settingsdialog.h

SOURCES += soxresampler.cpp \
 effectsoxrfactory.cpp \
 settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Effect/soxr
QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libsoxr.so

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target
    PKGCONFIG += soxr
    LIBS += -L/usr/lib -I/usr/include
}

win32 {
    LIBS += -lsoxr
}

FORMS += settingsdialog.ui

