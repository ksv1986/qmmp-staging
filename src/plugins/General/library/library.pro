include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/library

QT += sql concurrent

HEADERS += libraryfactory.h \
    library.h

SOURCES += libraryfactory.cpp \
    library.cpp


#RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
