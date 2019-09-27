include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/lyrics

QT += network

HEADERS += lyricsfactory.h \
           lyrics.h \
           lyricswindow.h \
    lyricsprovider.h \
    ultimatelyricsparser.h

SOURCES += lyricsfactory.cpp \
           lyrics.cpp \
           lyricswindow.cpp \
    lyricsprovider.cpp \
    ultimatelyricsparser.cpp

FORMS += lyricswindow.ui

RESOURCES = translations/translations.qrc \
            providers/providers.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
