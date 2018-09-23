include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/history

QT += sql

HEADERS += historyfactory.h \
    history.h \
    historywindow.h \
    dateinputdialog.h \
    historysettingsdialog.h \
    progressbaritemdelegate.h

SOURCES += historyfactory.cpp \
    history.cpp \
    historywindow.cpp \
    dateinputdialog.cpp \
    historysettingsdialog.cpp \
    progressbaritemdelegate.cpp

FORMS += \
    historywindow.ui \
    dateinputdialog.ui \
    historysettingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
