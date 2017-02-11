include(../../plugins.pri)
FORMS += settingsdialog.ui

HEADERS += outputoss4factory.h \
           outputoss4.h \
           settingsdialog.h


SOURCES += outputoss4factory.cpp \
           outputoss4.cpp \
           settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Output/oss4
QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/liboss4.so


CONFIG += warn_on \
thread \
plugin

DEFINES += HAVE_SYS_SOUNDCARD_H

TEMPLATE = lib
LIBS += -lqmmp

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
