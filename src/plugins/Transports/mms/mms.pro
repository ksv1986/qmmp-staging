include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Transports/mms

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

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += -L/usr/lib
PKGCONFIG += libmms

target.path = $$PLUGIN_DIR/Transports
INSTALLS += target

QMAKE_CFLAGS_ISYSTEM='' #removes "-isystem /usr/include"  provided by libmms.pc
