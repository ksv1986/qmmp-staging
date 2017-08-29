include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/qtmultimedia

QT += multimedia

FORMS += settingsdialog.ui

HEADERS += outputqtmultimediafactory.h \
           outputqtmultimedia.h \
           settingsdialog.h

SOURCES += outputqtmultimediafactory.cpp \
           outputqtmultimedia.cpp \
           settingsdialog.cpp

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Output
    INSTALLS += target
}
