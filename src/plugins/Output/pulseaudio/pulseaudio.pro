include(../../plugins.pri)

HEADERS += outputpulseaudiofactory.h \
           outputpulseaudio.h

SOURCES += outputpulseaudiofactory.cpp \
           outputpulseaudio.cpp


TARGET=$$PLUGINS_PREFIX/Output/pulseaudio
QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libpulseaudio.so

PKGCONFIG += libpulse-simple

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
