include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/playlistoption

HEADERS += playlistoption.h

SOURCES += playlistoption.cpp

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

target.path = $$PLUGIN_DIR/CommandLineOptions
INSTALLS += target
