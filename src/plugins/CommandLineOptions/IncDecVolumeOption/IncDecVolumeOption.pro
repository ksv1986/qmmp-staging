include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/incdecvolumeoption

CONFIG += link_qmmpui

HEADERS += incdecvolumeoption.h

SOURCES += incdecvolumeoption.cpp

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target
