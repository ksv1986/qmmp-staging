include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/General/copypaste
unix : QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libcopypaste.so

LIBS += -lqmmpui

RESOURCES = translations/translations.qrc
unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
HEADERS += copypastefactory.h \
           copypaste.h

SOURCES += copypastefactory.cpp \
           copypaste.cpp

