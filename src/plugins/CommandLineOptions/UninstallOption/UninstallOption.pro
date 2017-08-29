include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/uninstalloption

RESOURCES = translations/translations.qrc

LIBS += -lqmmpui

HEADERS += uninstalloption.h

SOURCES += uninstalloption.cpp
