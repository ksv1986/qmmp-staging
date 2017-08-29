include(../../plugins.pri)

HEADERS += outputjackfactory.h \
           outputjack.h \
           bio2jack.h

SOURCES += outputjackfactory.cpp \
           outputjack.cpp \
           bio2jack.c

TARGET=$$PLUGINS_PREFIX/Output/jack
QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libjack.so

contains(CONFIG, WITH_NEW_JACK){
    DEFINES += JACK_NEW_API
}

PKGCONFIG += jack soxr

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
