include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Transports/http

HEADERS += \
    httpinputfactory.h \
    httpinputsource.h \
    settingsdialog.h \
    httpstreamreader.h

SOURCES += \
    httpinputfactory.cpp \
    httpinputsource.cpp \
    settingsdialog.cpp \
    httpstreamreader.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

contains(CONFIG, WITH_ENCA){
   DEFINES += WITH_ENCA
   unix:PKGCONFIG += enca
   win32:LIBS += -lenca.dll
}

unix {
    LIBS += -L/usr/lib
    PKGCONFIG += libcurl
    target.path = $$LIB_DIR/qmmp/Transports
    INSTALLS += target
}
win32 {
    LIBS += -lcurldll
}


