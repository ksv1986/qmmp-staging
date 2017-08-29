include(../../plugins.pri)

HEADERS += decodersndfilefactory.h \
           decoder_sndfile.h
SOURCES += decoder_sndfile.cpp \
           decodersndfilefactory.cpp

TARGET=$$PLUGINS_PREFIX/Input/sndfile

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += sndfile
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libsndfile.so
}

win32 {
    LIBS += -lsndfile -lflac -lvorbisenc -lvorbis -logg
}
