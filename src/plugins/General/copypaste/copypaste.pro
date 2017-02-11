include(../../plugins.pri)

CONFIG += warn_on \
plugin

TARGET =$$PLUGINS_PREFIX/General/copypaste
unix : QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libcopypaste.so


TEMPLATE = lib
unix:LIBS += -lqmmpui -lqmmp

win32:LIBS += -lqmmpui0 -lqmmp0

RESOURCES = translations/translations.qrc
unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
HEADERS += copypastefactory.h \
           copypaste.h

win32 : HEADERS += ../../../../src/qmmpui/general.h
SOURCES += copypastefactory.cpp \
           copypaste.cpp

