include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/playlistoption

HEADERS += playlistoption.h

SOURCES += playlistoption.cpp

RESOURCES = translations/translations.qrc

LIBS += -lqmmpui

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target
