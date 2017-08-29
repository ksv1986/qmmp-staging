include(../../plugins.pri)

QT += dbus

TARGET =$$PLUGINS_PREFIX/General/mpris
QMAKE_CLEAN =$$PLUGINS_PREFIX/General/libmpris.so

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/General
INSTALLS += target

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

LIBS += -lqmmpui
