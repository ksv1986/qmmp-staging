include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/lyrics

QT += network

HEADERS += lyricsfactory.h \
           lyrics.h \
           lyricswindow.h \
    lyricsprovider.h \
    ultimatelyricsparser.h \
    settingsdialog.h

SOURCES += lyricsfactory.cpp \
           lyrics.cpp \
           lyricswindow.cpp \
    lyricsprovider.cpp \
    ultimatelyricsparser.cpp \
    settingsdialog.cpp

FORMS += lyricswindow.ui \
    settingsdialog.ui

RESOURCES = translations/translations.qrc \
            providers/providers.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
