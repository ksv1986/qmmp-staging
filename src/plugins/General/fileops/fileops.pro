include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/fileops

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

RESOURCES = translations/translations.qrc

LIBS += -lqmmpui

unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
