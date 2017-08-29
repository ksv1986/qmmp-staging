include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Effect/soxr

HEADERS += soxresampler.h \
 effectsoxrfactory.h \
 settingsdialog.h

SOURCES += soxresampler.cpp \
 effectsoxrfactory.cpp \
 settingsdialog.cpp

FORMS += settingsdialog.ui

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
