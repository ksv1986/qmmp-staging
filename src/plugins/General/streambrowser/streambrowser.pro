include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/General/streambrowser
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libstreambrowser.so

LIBS += -lqmmpui

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

