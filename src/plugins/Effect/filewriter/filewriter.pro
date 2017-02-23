include(../../plugins.pri)

HEADERS += effectfilewriterfactory.h \
    filewriterplugin.h \
    settingsdialog.h

SOURCES += effectfilewriterfactory.cpp \
    filewriterplugin.cpp \
    settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Effect/filewriter

CONFIG += warn_on plugin link_pkgconfig

TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target

    LIBS += -lqmmp
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libfilewriter.so
    PKGCONFIG += ogg vorbis vorbisenc
}

win32 {
    LIBS += -lqmmp0
}

FORMS += \
    settingsdialog.ui

