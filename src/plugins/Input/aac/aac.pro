include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/aac

HEADERS += decoderaacfactory.h \
    decoder_aac.h \
    aacfile.h \
    aacmetadatamodel.h
SOURCES += decoder_aac.cpp \
    decoderaacfactory.cpp \
    aacfile.cpp \
    aacmetadatamodel.cpp

RESOURCES = translations/translations.qrc

LIBS += -lfaad \
    -L/usr/lib \
    -I/usr/include
PKGCONFIG += taglib

target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
