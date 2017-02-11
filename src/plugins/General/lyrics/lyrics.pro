include(../../plugins.pri)

CONFIG += warn_on \
plugin

TARGET =$$PLUGINS_PREFIX/General/lyrics
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/liblyrics.so


TEMPLATE = lib
unix:LIBS += -lqmmpui -lqmmp

win32:LIBS += -lqmmpui0 -lqmmp0

RESOURCES = translations/translations.qrc

unix {
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
}
HEADERS += lyricsfactory.h \
           lyrics.h \
           lyricswindow.h
win32:HEADERS += ../../../../src/qmmpui/general.h
SOURCES += lyricsfactory.cpp \
           lyrics.cpp \
           lyricswindow.cpp

FORMS += lyricswindow.ui

QT += network

