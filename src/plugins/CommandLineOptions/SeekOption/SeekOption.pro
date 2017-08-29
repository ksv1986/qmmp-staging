include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/CommandLineOptions/seekoption
QMAKE_CLEAN = $$PLUGINS_PREFIX/CommandLineOptions/libseekoption.so

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/CommandLineOptions
INSTALLS += target

LIBS += -lqmmpui

HEADERS += seekoption.h

SOURCES += seekoption.cpp
