include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/statusicon

HEADERS += statusiconfactory.h \
    statusicon.h \
    settingsdialog.h \
    qmmptrayicon.h \
    coverwidget.h

SOURCES += statusiconfactory.cpp \
    statusicon.cpp \
    settingsdialog.cpp \
    qmmptrayicon.cpp \
    coverwidget.cpp

contains(DEFINES, QMMP_WS_X11) {
    HEADERS += statusiconpopupwidget.h
    SOURCES += statusiconpopupwidget.cpp
}

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc \
            images/tray_images.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
