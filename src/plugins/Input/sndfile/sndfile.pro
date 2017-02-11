include(../../plugins.pri)

HEADERS += decodersndfilefactory.h \
           decoder_sndfile.h
SOURCES += decoder_sndfile.cpp \
           decodersndfilefactory.cpp

TARGET=$$PLUGINS_PREFIX/Input/sndfile

CONFIG += warn_on \
plugin \
link_pkgconfig

TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    LIBS += -lqmmp
    PKGCONFIG += sndfile
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libsndfile.so
}

win32 {
    HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
    LIBS += -lqmmp0 -lsndfile -lflac -lvorbisenc -lvorbis -logg
}
