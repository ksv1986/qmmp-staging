include(../../plugins.pri)
FORMS += settingsdialog.ui
HEADERS += decodermadfactory.h \
    decoder_mad.h \
    settingsdialog.h \
    tagextractor.h \
    mpegmetadatamodel.h \
    replaygainreader.h
SOURCES += decoder_mad.cpp \
    decodermadfactory.cpp \
    settingsdialog.cpp \
    tagextractor.cpp \
    mpegmetadatamodel.cpp \
    replaygainreader.cpp
TARGET = $$PLUGINS_PREFIX/Input/mad

RESOURCES = translations/translations.qrc
unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target

    LIBS += -lmad
    PKGCONFIG += taglib mad
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libmad.so
}

win32 {
    LIBS += -lmad -ltag.dll
}
