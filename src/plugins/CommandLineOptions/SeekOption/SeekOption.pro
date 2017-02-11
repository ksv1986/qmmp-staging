include(../../plugins.pri)

CONFIG += warn_on \
plugin  \
 lib

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/seekoption
QMAKE_CLEAN = $$PLUGINS_PREFIX/CommandLineOptions/libseekoption.so

TEMPLATE = lib

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

LIBS += -lqmmpui -lqmmp

HEADERS += seekoption.h

SOURCES += seekoption.cpp
