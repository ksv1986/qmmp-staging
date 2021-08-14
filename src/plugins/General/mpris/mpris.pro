include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/mpris

QT += dbus

HEADERS += mprisfactory.h \
           mpris.h  \
           root2object.h \
           player2object.h

SOURCES += mprisfactory.cpp \
           mpris.cpp \
           root2object.cpp \
           player2object.cpp

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

target.path = $$PLUGIN_DIR/General
INSTALLS += target
