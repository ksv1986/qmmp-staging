include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/streambrowser

QT += network

HEADERS += streambrowserfactory.h \
           streambrowser.h \
           streamwindow.h \
    editstreamdialog.h

SOURCES += streambrowserfactory.cpp \
           streambrowser.cpp \
           streamwindow.cpp \
    editstreamdialog.cpp

FORMS += streamwindow.ui \
    editstreamdialog.ui

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
  target.path = $$PLUGIN_DIR/General
  INSTALLS += target
}
