include(../../plugins.pri)

CONFIG += warn_on \
plugin  \
 lib

TARGET =$$PLUGINS_PREFIX/CommandLineOptions/playlistoption
QMAKE_CLEAN =$$PLUGINS_PREFIX/CommandLineOptions/libplaylistoption.so

TEMPLATE = lib

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

LIBS += -lqmmpui -lqmmp

HEADERS += playlistoption.h

SOURCES += playlistoption.cpp
