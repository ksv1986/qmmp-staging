include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Effect/bs2b

HEADERS += bs2bplugin.h \
           effectbs2bfactory.h \
           settingsdialog.h

SOURCES += bs2bplugin.cpp \
           effectbs2bfactory.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target
    PKGCONFIG += libbs2b
    LIBS += -L/usr/lib -I/usr/include
}

win32 {
    LIBS += -lbs2b
}
