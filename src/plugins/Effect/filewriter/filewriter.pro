include(../../plugins.pri)

HEADERS += effectfilewriterfactory.h \
    filewriterplugin.h \
    settingsdialog.h

SOURCES += effectfilewriterfactory.cpp \
    filewriterplugin.cpp \
    settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Effect/filewriter

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target

    LIBS += -lqmmp -lqmmpui
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libfilewriter.so
    PKGCONFIG += ogg vorbis vorbisenc
}

win32 {
    LIBS += -lqmmp0 -lqmmpui0 -lvorbisfile -lvorbis -logg -lvorbisenc
    LD_FLAGS += -no-undefined
}

FORMS += \
    settingsdialog.ui

