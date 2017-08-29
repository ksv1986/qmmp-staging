include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/directsound

HEADERS += outputdirectsoundfactory.h \
           outputdirectsound.h

SOURCES += outputdirectsoundfactory.cpp \
           outputdirectsound.cpp

RESOURCES = translations/translations.qrc

LIBS += -ldxguid -lstrmiids -ldmoguids -lmsdmo -lole32 -loleaut32 -luuid -lgdi32 -ldsound -lksuser

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
