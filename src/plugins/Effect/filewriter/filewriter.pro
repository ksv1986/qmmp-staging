include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Effect/filewriter

HEADERS += effectfilewriterfactory.h \
           filewriterplugin.h \
           settingsdialog.h

SOURCES += effectfilewriterfactory.cpp \
           filewriterplugin.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += -lqmmpui

unix {
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target
    PKGCONFIG += ogg vorbis vorbisenc
}

win32 {
    LIBS += -lvorbisfile -lvorbis -logg -lvorbisenc
    LD_FLAGS += -no-undefined
}
