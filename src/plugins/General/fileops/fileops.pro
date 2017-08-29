include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/General/fileops
unix : QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libfileops.so

LIBS += -lqmmpui

RESOURCES = translations/translations.qrc
unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
HEADERS += fileopsfactory.h \
           fileops.h \
           settingsdialog.h \
           hotkeydialog.h

SOURCES += fileopsfactory.cpp \
           fileops.cpp \
           settingsdialog.cpp \
           hotkeydialog.cpp

FORMS += settingsdialog.ui \
         hotkeydialog.ui


