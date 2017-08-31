include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/uninstalloption

HEADERS += uninstalloption.h

SOURCES += uninstalloption.cpp

LIBS += $$QMMPUI_LIB

RESOURCES = translations/translations.qrc
