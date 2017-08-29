include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/oss

FORMS += settingsdialog.ui

HEADERS += outputossfactory.h \
           outputoss.h \
           settingsdialog.h

SOURCES += outputossfactory.cpp \
           outputoss.cpp \
           settingsdialog.cpp

RESOURCES = translations/translations.qrc

DEFINES += HAVE_SYS_SOUNDCARD_H

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
