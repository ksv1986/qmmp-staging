include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/streambrowser

QT += network
CONFIG += link_qmmpui

HEADERS += streambrowserfactory.h \
           streambrowser.h \
           streamwindow.h

SOURCES += streambrowserfactory.cpp \
           streambrowser.cpp \
           streamwindow.cpp

FORMS += streamwindow.ui

RESOURCES = translations/translations.qrc

unix {
  target.path = $$LIB_DIR/qmmp/General
  INSTALLS += target
}
