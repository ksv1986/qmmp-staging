include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Engines/mplayer

HEADERS += mplayerenginefactory.h \
    mplayerengine.h \
    settingsdialog.h \
    mplayermetadatamodel.h

SOURCES += mplayerengine.cpp \
    mplayerenginefactory.cpp \
    settingsdialog.cpp \
    mplayermetadatamodel.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/Engines
INSTALLS += target
