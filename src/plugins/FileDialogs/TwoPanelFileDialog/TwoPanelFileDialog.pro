include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/FileDialogs/twopanelfiledialog

HEADERS += twopanelfiledialog.h \
           twopanelfiledialogimpl.h

SOURCES += twopanelfiledialog.cpp \
           twopanelfiledialogimpl.cpp


FORMS += twopanelfiledialog.ui

LIBS += -lqmmpui

RESOURCES += translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/FileDialogs
    INSTALLS += target
}
