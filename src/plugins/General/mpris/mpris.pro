include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/mpris

QT += dbus
CONFIG += link_qmmpui

HEADERS += mprisfactory.h \
           mpris.h  \
           mpris1/playerobject.h \
           mpris1/rootobject.h \
           mpris1/tracklistobject.h \
           mpris2/root2object.h \
           mpris2/player2object.h

SOURCES += mprisfactory.cpp \
           mpris.cpp \
           mpris1/playerobject.cpp \
           mpris1/rootobject.cpp \
           mpris1/tracklistobject.cpp \
           mpris2/root2object.cpp \
           mpris2/player2object.cpp

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
