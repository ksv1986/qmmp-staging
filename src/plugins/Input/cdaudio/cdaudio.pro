include(../../plugins.pri)

HEADERS += decodercdaudiofactory.h \
           decoder_cdaudio.h \
           settingsdialog.h
SOURCES += decoder_cdaudio.cpp \
           decodercdaudiofactory.cpp \
           settingsdialog.cpp

CONFIG += warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib

TARGET =$$PLUGINS_PREFIX/Input/cdaudio

RESOURCES = translations/translations.qrc


FORMS += settingsdialog.ui

unix {
  LIBS += -lqmmp -L/usr/lib  -I/usr/include
  PKGCONFIG += libcdio libcdio_cdda libcdio_paranoia libcddb
  target.path = $$LIB_DIR/qmmp/Input
  QMAKE_CLEAN =$$PLUGINS_PREFIX/Input/libcdaudio.so
  INSTALLS += target
}

win32 {
  HEADERS += ../../../../src/qmmp/metadatamodel.h \
               ../../../../src/qmmp/decoderfactory.h
  LIBS += -lqmmp0 -lcdio -lcdio_paranoia -lcdio_cdda  -lm -lwinmm -mwindows -liconv -lcddb -lws2_32
}
