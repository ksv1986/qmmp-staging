include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/trackchange

HEADERS += trackchangefactory.h \
           trackchange.h \
           settingsdialog.h

SOURCES += trackchangefactory.cpp \
           trackchange.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
