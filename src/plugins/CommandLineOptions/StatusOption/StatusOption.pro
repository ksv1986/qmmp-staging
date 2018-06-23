include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/statusoption

HEADERS += statusoption.h

SOURCES += statusoption.cpp

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

target.path = $$PLUGIN_DIR/CommandLineOptions
INSTALLS += target
