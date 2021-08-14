include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/aac

HEADERS += decoderaacfactory.h \
    decoder_aac.h \
    aacfile.h
SOURCES += decoder_aac.cpp \
    decoderaacfactory.cpp \
    aacfile.cpp

RESOURCES = translations/translations.qrc

LIBS += -lfaad \
    -L/usr/lib \
    -I/usr/include
PKGCONFIG += taglib

target.path = $$PLUGIN_DIR/Input
INSTALLS += target
