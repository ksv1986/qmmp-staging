include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/sndfile

HEADERS += decodersndfilefactory.h \
           decoder_sndfile.h

SOURCES += decoder_sndfile.cpp \
           decodersndfilefactory.cpp

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    PKGCONFIG += sndfile
}

win32 {
    LIBS += -lsndfile -lflac -lvorbisenc -lvorbis -logg
}
