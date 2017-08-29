include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/seekoption

HEADERS += seekoption.h

SOURCES += seekoption.cpp

LIBS += -lqmmpui

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target
