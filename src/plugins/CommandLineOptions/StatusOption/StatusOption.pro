include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/statusoption

CONFIG += link_qmmpui

HEADERS += statusoption.h

SOURCES += statusoption.cpp

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target
