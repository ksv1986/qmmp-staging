include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/taskbar

QT += winextras

HEADERS += taskbarfactory.h \
           taskbarhelper.h

SOURCES += taskbarfactory.cpp \
           taskbarhelper.cpp 

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

target.path = $$PLUGIN_DIR/General
INSTALLS += target

