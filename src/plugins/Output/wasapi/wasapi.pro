include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/wasapi

HEADERS += outputwasapifactory.h \
           outputwasapi.h \
    settingsdialog.h

SOURCES += outputwasapifactory.cpp \
           outputwasapi.cpp \
    settingsdialog.cpp

RESOURCES = translations/translations.qrc

LIBS += -lstrmiids -ldmoguids -lmsdmo -lole32 -loleaut32 -luuid -lgdi32 -lksuser

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target

FORMS += \
    settingsdialog.ui
