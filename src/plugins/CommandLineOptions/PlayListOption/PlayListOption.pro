include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/CommandLineOptions/playlistoption
QMAKE_CLEAN =$$PLUGINS_PREFIX/CommandLineOptions/libplaylistoption.so

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

LIBS += -lqmmpui

HEADERS += playlistoption.h

SOURCES += playlistoption.cpp
