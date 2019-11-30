include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/General/sleepinhibitor

QT += dbus

TEMPLATE = lib
LIBS += $${QMMPUI_LIB}


HEADERS += sleepinhibitorfactory.h \
           sleepinhibitor.h

SOURCES += sleepinhibitorfactory.cpp \
           sleepinhibitor.cpp

#RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
