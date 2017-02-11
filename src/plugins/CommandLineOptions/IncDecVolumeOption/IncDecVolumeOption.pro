include(../../plugins.pri)

CONFIG += warn_on \
plugin  \
 lib

TARGET =$$PLUGINS_PREFIX/CommandLineOptions/incdecvolumeoption
QMAKE_CLEAN =$$PLUGINS_PREFIX/CommandLineOptions/libincdecvolumeoption.so

TEMPLATE = lib

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

LIBS += -lqmmpui -lqmmp

HEADERS += incdecvolumeoption.h

SOURCES += incdecvolumeoption.cpp
