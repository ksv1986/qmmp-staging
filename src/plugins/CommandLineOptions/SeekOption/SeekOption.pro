include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/seekoption

HEADERS += seekoption.h

SOURCES += seekoption.cpp

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target
