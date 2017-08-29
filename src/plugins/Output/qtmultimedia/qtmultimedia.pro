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


RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Output
    INSTALLS += target
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libqtmultimedia.so
}
