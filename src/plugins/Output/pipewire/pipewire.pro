include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/pipewire

HEADERS += outputpipewirefactory.h \
           outputpipewire.h

SOURCES += outputpipewirefactory.cpp \
           outputpipewire.cpp

INCLUDEPATH += /usr/include/pipewire-0.3 /usr/include/spa-0.2

RESOURCES = translations/translations.qrc

PKGCONFIG += libpipewire-0.3 libspa-0.2

QMAKE_CXXFLAGS += -Wno-missing-field-initializers

target.path = $$PLUGIN_DIR/Output
INSTALLS += target
