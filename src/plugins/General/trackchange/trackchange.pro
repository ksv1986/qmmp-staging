include(../../plugins.pri)

CONFIG += warn_on \
plugin

TARGET =$$PLUGINS_PREFIX/General/trackchange
unix : QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libtrackchange.so


TEMPLATE = lib
LIBS += $${QMMPUI_LIB}

RESOURCES = translations/translations.qrc
unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
HEADERS += trackchangefactory.h \
           trackchange.h \
           settingsdialog.h

win32 : HEADERS += ../../../../src/qmmpui/general.h
SOURCES += trackchangefactory.cpp \
           trackchange.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui
