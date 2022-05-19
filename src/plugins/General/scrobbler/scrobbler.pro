include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/scrobbler

QT += network

HEADERS += scrobblerfactory.h \
    settingsdialog.h \
    scrobblerhandler.h \
    scrobblercache.h \
    scrobbler.h \
    defines.h

SOURCES += scrobblerfactory.cpp \
    settingsdialog.cpp \
    scrobblerhandler.cpp \
    scrobblercache.cpp \
    scrobbler.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
