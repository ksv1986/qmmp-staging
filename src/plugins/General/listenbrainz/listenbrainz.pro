include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/listenbrainz

QT += network

HEADERS += listenbrainzfactory.h \
           settingsdialog.h \
           listenbrainz.h \
           payloadcache.h

SOURCES += listenbrainzfactory.cpp \
           settingsdialog.cpp \
           listenbrainz.cpp \
           payloadcache.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
