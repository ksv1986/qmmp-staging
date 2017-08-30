include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/FileDialogs/qmmpfiledialog

CONFIG += link_qmmpui

HEADERS += qmmpfiledialog.h \
           qmmpfiledialogimpl.h

SOURCES += qmmpfiledialog.cpp \
           qmmpfiledialogimpl.cpp

FORMS += qmmpfiledialog.ui

RESOURCES += translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/FileDialogs
    INSTALLS += target
}
