include(../../plugins.pri)

CONFIG += warn_on \
plugin  \
 lib

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/statusoption
QMAKE_CLEAN = $$PLUGINS_PREFIX/CommandLineOptions/libstatusoption.so

TEMPLATE = lib

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

LIBS += -lqmmpui -lqmmp

HEADERS += statusoption.h

SOURCES += statusoption.cpp
