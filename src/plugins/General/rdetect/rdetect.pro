include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/rdetect

HEADERS += rdetectfactory.h \ 
    removablehelper.h

SOURCES += rdetectfactory.cpp \ 
    removablehelper.cpp

#RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

target.path = $$PLUGIN_DIR/General
INSTALLS += target
