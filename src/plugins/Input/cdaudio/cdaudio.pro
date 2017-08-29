include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/cdaudio

HEADERS += decodercdaudiofactory.h \
           decoder_cdaudio.h \
           settingsdialog.h

SOURCES += decoder_cdaudio.cpp \
           decodercdaudiofactory.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += -lqmmp

unix {
  LIBS += -L/usr/lib  -I/usr/include
  PKGCONFIG += libcdio libcdio_cdda libcdio_paranoia libcddb
  target.path = $$LIB_DIR/qmmp/Input
  INSTALLS += target
}

win32 {
  LIBS += -lcdio -lcdio_paranoia -lcdio_cdda  -lm -lwinmm -mwindows -liconv -lcddb -lws2_32
}
