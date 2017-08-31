include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/statusicon

HEADERS += statusiconfactory.h \
    statusicon.h \
    settingsdialog.h \
    qmmptrayicon.h \
    statusiconpopupwidget.h \
    coverwidget.h

SOURCES += statusiconfactory.cpp \
    statusicon.cpp \
    settingsdialog.cpp \
    qmmptrayicon.cpp \
    statusiconpopupwidget.cpp \
    coverwidget.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc \
            images/tray_images.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
