include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/uninstalloption

CONFIG += link_qmmpui

HEADERS += uninstalloption.h

SOURCES += uninstalloption.cpp

RESOURCES = translations/translations.qrc
