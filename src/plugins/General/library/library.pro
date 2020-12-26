include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/library

QT += sql concurrent

HEADERS += libraryfactory.h \
    library.h \
    settingsdialog.h \
    librarywidget.h \
    librarymodel.h

SOURCES += libraryfactory.cpp \
    library.cpp \
    settingsdialog.cpp \
    librarywidget.cpp \
    librarymodel.cpp


#RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}

FORMS += \
    settingsdialog.ui \
    librarywidget.ui
