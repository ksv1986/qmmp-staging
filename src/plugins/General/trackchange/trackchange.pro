include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/General/trackchange
unix : QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libtrackchange.so

LIBS += -lqmmpui

RESOURCES = translations/translations.qrc
unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
HEADERS += trackchangefactory.h \
           trackchange.h \
           settingsdialog.h

SOURCES += trackchangefactory.cpp \
           trackchange.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui
