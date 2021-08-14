include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/waveout

HEADERS += outputwaveoutfactory.h \
           outputwaveout.h

SOURCES += outputwaveoutfactory.cpp \
           outputwaveout.cpp

RESOURCES = translations/translations.qrc

LIBS += -lwinmm

target.path = $$PLUGIN_DIR/Output
INSTALLS += target
