include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/FileDialogs/twopanelfiledialog

HEADERS += twopanelfiledialog.h \
           twopanelfiledialogimpl.h

SOURCES += twopanelfiledialog.cpp \
           twopanelfiledialogimpl.cpp


FORMS += twopanelfiledialog.ui

LIBS += $$QMMPUI_LIB

RESOURCES += translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/FileDialogs
    INSTALLS += target
}
