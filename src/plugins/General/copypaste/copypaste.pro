include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/copypaste

HEADERS += copypastefactory.h \
           copypaste.h

SOURCES += copypastefactory.cpp \
           copypaste.cpp

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
