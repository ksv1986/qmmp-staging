include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/lyrics

QT += network

HEADERS += lyricsfactory.h \
           lyrics.h \
    lyricsprovider.h \
    ultimatelyricsparser.h \
    settingsdialog.h \
    lyricswidget.h

SOURCES += lyricsfactory.cpp \
           lyrics.cpp \
    lyricsprovider.cpp \
    ultimatelyricsparser.cpp \
    settingsdialog.cpp \
    lyricswidget.cpp

FORMS += \
    settingsdialog.ui \
    lyricswidget.ui

RESOURCES = translations/translations.qrc \
            providers/providers.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
