include(../../plugins.pri)

HEADERS += decodercdaudiofactory.h \
           decoder_cdaudio.h \
           settingsdialog.h
SOURCES += decoder_cdaudio.cpp \
           decodercdaudiofactory.cpp \
           settingsdialog.cpp

TARGET =$$PLUGINS_PREFIX/Input/cdaudio

RESOURCES = translations/translations.qrc

FORMS += settingsdialog.ui

LIBS += -lqmmp

unix {
  LIBS += -L/usr/lib  -I/usr/include
  PKGCONFIG += libcdio libcdio_cdda libcdio_paranoia libcddb
  target.path = $$LIB_DIR/qmmp/Input
  QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libcdaudio.so
  INSTALLS += target
}

win32 {
  LIBS += -lcdio -lcdio_paranoia -lcdio_cdda  -lm -lwinmm -mwindows -liconv -lcddb -lws2_32
}
