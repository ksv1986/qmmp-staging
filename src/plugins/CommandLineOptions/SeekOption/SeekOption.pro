include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/seekoption

CONFIG += link_qmmpui

HEADERS += seekoption.h

SOURCES += seekoption.cpp

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target
