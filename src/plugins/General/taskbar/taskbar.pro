include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/taskbar

HEADERS += taskbarfactory.h \
           taskbarhelper.h \
    qwintaskbarbutton.h \
    qwintaskbarbutton_p.h \
    qwintaskbarprogress.h \
    qwinevent.h \
    qwineventfilter_p.h

SOURCES += taskbarfactory.cpp \
           taskbarhelper.cpp \ 
    qwintaskbarbutton.cpp \
    qwintaskbarprogress.cpp \
    qwinevent.cpp \
    qwineventfilter.cpp

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB -lole32 -lshlwapi -lshell32 -ldwmapi -lgdi32 -luuid

target.path = $$PLUGIN_DIR/General
INSTALLS += target

