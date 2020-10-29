include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/library

QT += sql

HEADERS += libraryfactory.h

SOURCES += libraryfactory.cpp


#RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
