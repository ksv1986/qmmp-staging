include(../../plugins.pri)

CONFIG += warn_on \
plugin

TARGET =$$PLUGINS_PREFIX/General/streambrowser
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libstreambrowser.so


TEMPLATE = lib
unix:LIBS += -lqmmpui -lqmmp

win32:LIBS += -lqmmpui0 -lqmmp0

RESOURCES = translations/translations.qrc

unix {
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
}
HEADERS += streambrowserfactory.h \
           streambrowser.h \
           streamwindow.h
win32:HEADERS += ../../../../src/qmmpui/general.h
SOURCES += streambrowserfactory.cpp \
           streambrowser.cpp \
           streamwindow.cpp

FORMS += streamwindow.ui

QT += network

