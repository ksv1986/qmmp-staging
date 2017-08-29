include(../../plugins.pri)
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

TARGET = $$PLUGINS_PREFIX/Transports/http
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libhttp.so

unix {
    LIBS += -L/usr/lib
    PKGCONFIG += libcurl
    target.path = $$LIB_DIR/qmmp/Transports
    INSTALLS += target
}
win32 {
    LIBS += -lcurldll
}

contains(CONFIG, WITH_ENCA){
   DEFINES += WITH_ENCA
   unix:PKGCONFIG += enca
   win32:LIBS += -lenca.dll

}

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc
