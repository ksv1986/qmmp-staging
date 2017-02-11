include(../../plugins.pri)

QT += multimedia

FORMS += settingsdialog.ui

HEADERS += outputqtmultimediafactory.h \
           outputqtmultimedia.h \
           settingsdialog.h

SOURCES += outputqtmultimediafactory.cpp \
           outputqtmultimedia.cpp \
           settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Output/qtmultimedia

CONFIG += warn_on \
thread \
plugin

TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Output
    INSTALLS += target
    LIBS += -lqmmp
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libqtmultimedia.so
}

win32 {
    LIBS += -lqmmp0
}
