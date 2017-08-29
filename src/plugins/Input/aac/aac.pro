include(../../plugins.pri)
FORMS +=
HEADERS += decoderaacfactory.h \
    decoder_aac.h \
    aacfile.h \
    aacmetadatamodel.h
SOURCES += decoder_aac.cpp \
    decoderaacfactory.cpp \
    aacfile.cpp \
    aacmetadatamodel.cpp
TARGET = $$PLUGINS_PREFIX/Input/aac
QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libaac.so
LIBS += -lfaad \
    -L/usr/lib \
    -I/usr/include
PKGCONFIG += taglib

RESOURCES = translations/translations.qrc
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
