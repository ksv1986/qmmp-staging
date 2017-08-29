include(../../plugins.pri)

unix:TARGET = $$PLUGINS_PREFIX/General/statusicon
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libstatusicon.so
win32:TARGET = $$PLUGINS_PREFIX/General/statusicon

LIBS += -lqmmpui

RESOURCES = translations/translations.qrc
unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
RESOURCES += images/tray_images.qrc
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

