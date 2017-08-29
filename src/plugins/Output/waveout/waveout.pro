include(../../plugins.pri)

HEADERS += outputwaveoutfactory.h \
           outputwaveout.h

SOURCES += outputwaveoutfactory.cpp \
           outputwaveout.cpp

TARGET=$$PLUGINS_PREFIX/Output/waveout

LIBS += -lwinmm

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
