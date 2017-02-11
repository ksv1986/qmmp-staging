include(../../plugins.pri)
HEADERS += \
    mmsinputfactory.h \
    mmsinputsource.h \
    settingsdialog.h \
    mmsstreamreader.h
SOURCES += \
    mmsinputfactory.cpp \
    mmsinputsource.cpp \
    settingsdialog.cpp \
    mmsstreamreader.cpp
win32:HEADERS += ../../../../src/qmmp/inputsource.h \
    ../../../../src/qmmp/inputsourcefactory.h
TARGET = $$PLUGINS_PREFIX/Transports/mms
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libmms.so
CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

LIBS += -lqmmp -L/usr/lib
PKGCONFIG += libmms
target.path = $$LIB_DIR/qmmp/Transports
INSTALLS += target

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

QMAKE_CFLAGS_ISYSTEM='' #removes "-isystem /usr/include"  provided by libmms.pc
