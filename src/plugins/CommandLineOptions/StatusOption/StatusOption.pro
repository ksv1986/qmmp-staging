include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/statusoption
QMAKE_CLEAN = $$PLUGINS_PREFIX/CommandLineOptions/libstatusoption.so

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

LIBS += -lqmmpui

HEADERS += statusoption.h

SOURCES += statusoption.cpp
