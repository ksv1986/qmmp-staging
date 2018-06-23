include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/mpc

HEADERS += decodermpcfactory.h \
    decoder_mpc.h \
    mpcmetadatamodel.h

SOURCES += decoder_mpc.cpp \
    decodermpcfactory.cpp \
    mpcmetadatamodel.cpp


RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    PKGCONFIG += taglib
    LIBS += -lmpcdec -I/usr/include
}

win32 {
    LIBS += -lmpcdec -ltag.dll
}
