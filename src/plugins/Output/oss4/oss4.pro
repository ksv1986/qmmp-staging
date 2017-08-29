include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/oss4

HEADERS += outputoss4factory.h \
           outputoss4.h \
           settingsdialog.h

SOURCES += outputoss4factory.cpp \
           outputoss4.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

DEFINES += HAVE_SYS_SOUNDCARD_H

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
