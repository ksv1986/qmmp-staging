include(../../plugins.pri)

CONFIG += warn_on \
plugin  \
 lib

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/uninstalloption

TEMPLATE = lib

RESOURCES = translations/translations.qrc

LIBS += -lqmmpui0 -lqmmp0

HEADERS += uninstalloption.h

SOURCES += uninstalloption.cpp
