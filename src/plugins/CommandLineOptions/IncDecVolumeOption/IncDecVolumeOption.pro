include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/CommandLineOptions/incdecvolumeoption
QMAKE_CLEAN =$$PLUGINS_PREFIX/CommandLineOptions/libincdecvolumeoption.so

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

LIBS += -lqmmpui

HEADERS += incdecvolumeoption.h

SOURCES += incdecvolumeoption.cpp
