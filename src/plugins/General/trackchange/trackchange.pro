include(../../plugins.pri)

CONFIG += warn_on \
plugin

TARGET =$$PLUGINS_PREFIX/General/trackchange
unix : QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libtrackchange.so


TEMPLATE = lib
unix : LIBS += -lqmmpui -lqmmp

win32 : LIBS += -lqmmpui0 -lqmmp0

RESOURCES = translations/translations.qrc
unix {
    target.path = $$LIB_DIR/qmmp/General
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
