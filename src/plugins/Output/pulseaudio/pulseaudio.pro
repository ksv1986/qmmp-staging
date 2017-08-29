include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/pulseaudio

HEADERS += outputpulseaudiofactory.h \
           outputpulseaudio.h

SOURCES += outputpulseaudiofactory.cpp \
           outputpulseaudio.cpp

RESOURCES = translations/translations.qrc

PKGCONFIG += libpulse-simple

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
