include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/mad

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

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
    LIBS += -lmad
    PKGCONFIG += taglib mad
}

win32 {
    LIBS += -lmad -ltag.dll
}
