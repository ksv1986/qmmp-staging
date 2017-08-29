include(../../plugins.pri)
FORMS += settingsdialog.ui

HEADERS += outputossfactory.h \
           outputoss.h \
           settingsdialog.h

SOURCES += outputossfactory.cpp \
           outputoss.cpp \
           settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Output/oss
QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/liboss.so

DEFINES += HAVE_SYS_SOUNDCARD_H

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
