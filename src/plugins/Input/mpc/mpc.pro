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
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += taglib
    LIBS += -lmpcdec  -I/usr/include
    exists(/usr/include/mpcdec/mpcdec.h):DEFINES += MPC_OLD_API
}

win32 {
    LIBS += -lmpcdec -ltag.dll
    #DEFINES += MPC_OLD_API
}
