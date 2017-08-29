include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/streambrowser

QT += network

HEADERS += streambrowserfactory.h \
           streambrowser.h \
           streamwindow.h

SOURCES += streambrowserfactory.cpp \
           streambrowser.cpp \
           streamwindow.cpp

FORMS += streamwindow.ui

RESOURCES = translations/translations.qrc

LIBS += -lqmmpui

unix {
  target.path = $$LIB_DIR/qmmp/General
  INSTALLS += target
}
