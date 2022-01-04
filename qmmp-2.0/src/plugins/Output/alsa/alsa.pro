include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/alsa

HEADERS += outputalsa.h \
           outputalsafactory.h  \
           settingsdialog.h
SOURCES += outputalsa.cpp \
           outputalsafactory.cpp  \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

PKGCONFIG += alsa

target.path = $$PLUGIN_DIR/Output
INSTALLS += target
