include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/jack

HEADERS += outputjackfactory.h \
           outputjack.h \
           bio2jack.h

SOURCES += outputjackfactory.cpp \
           outputjack.cpp \
           bio2jack.c

RESOURCES = translations/translations.qrc

PKGCONFIG += jack soxr

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
