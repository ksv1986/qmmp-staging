include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/playlistoption

CONFIG += link_qmmpui

HEADERS += playlistoption.h

SOURCES += playlistoption.cpp

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target
