include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Effect/stereo

HEADERS += stereoplugin.h \
           effectstereofactory.h \
           settingsdialog.h

SOURCES += stereoplugin.cpp \
           effectstereofactory.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Effect
    INSTALLS += target
}
