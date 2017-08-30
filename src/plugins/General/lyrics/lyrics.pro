include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/lyrics

QT += network
CONFIG += link_qmmpui

HEADERS += lyricsfactory.h \
           lyrics.h \
           lyricswindow.h

SOURCES += lyricsfactory.cpp \
           lyrics.cpp \
           lyricswindow.cpp

FORMS += lyricswindow.ui

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
