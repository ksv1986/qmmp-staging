include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/wavpack

HEADERS += decoderwavpackfactory.h \
    decoder_wavpack.h \
    wavpackmetadatamodel.h

SOURCES += decoder_wavpack.cpp \
    decoderwavpackfactory.cpp \
    wavpackmetadatamodel.cpp


RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    PKGCONFIG += wavpack
}

win32 {
    LIBS += -lwavpack
}
