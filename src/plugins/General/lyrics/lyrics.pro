include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/lyrics

QT += network

HEADERS += lyricsfactory.h \
           lyrics.h \
           lyricswindow.h

SOURCES += lyricsfactory.cpp \
           lyrics.cpp \
           lyricswindow.cpp

FORMS += lyricswindow.ui

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
