include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/FileDialogs/qmmpfiledialog

HEADERS += qmmpfiledialog.h \
           qmmpfiledialogimpl.h

SOURCES += qmmpfiledialog.cpp \
           qmmpfiledialogimpl.cpp

FORMS += qmmpfiledialog.ui

RESOURCES += translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/FileDialogs
    INSTALLS += target
}
