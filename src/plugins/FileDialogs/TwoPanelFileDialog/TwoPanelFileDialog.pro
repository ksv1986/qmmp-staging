include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/FileDialogs/twopanelfiledialog


HEADERS += twopanelfiledialog.h \
           twopanelfiledialogimpl.h

SOURCES += twopanelfiledialog.cpp \
           twopanelfiledialogimpl.cpp


FORMS += twopanelfiledialog.ui

CONFIG += warn_on plugin

TEMPLATE = lib

RESOURCES += translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/FileDialogs
    INSTALLS += target

    QMAKE_CLEAN =$$PLUGINS_PREFIX/FileDialogs/libtwopanelfiledialog.so
    LIBS += -lqmmpui -lqmmp
}

win32 {
    LIBS += -lqmmpui0 -lqmmp0
}
