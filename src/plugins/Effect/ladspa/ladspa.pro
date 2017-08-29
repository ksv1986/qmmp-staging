include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Effect/ladspa

HEADERS += ladspahost.h \
    effectladspafactory.h \
    settingsdialog.h \
    ladspaslider.h \
    ladspa.h \
    ladspahelper.h \
    ladspabutton.h

SOURCES += ladspahost.cpp \
    effectladspafactory.cpp \
    settingsdialog.cpp \
    ladspaslider.cpp \
    ladspahelper.cpp \
    ladspabutton.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += -L/usr/lib -I/usr/include
linux-g++|linux-g++-32|linux-g++-64:LIBS += -ldl

target.path = $$LIB_DIR/qmmp/Effect
INSTALLS += target
