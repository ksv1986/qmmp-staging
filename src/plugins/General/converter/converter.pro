include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/converter

HEADERS += converterfactory.h \
    converterhelper.h \
    converterdialog.h \
    converter.h \
    preseteditor.h

SOURCES += converterfactory.cpp \
    converterhelper.cpp \
    converterdialog.cpp \
    converter.cpp \
    preseteditor.cpp

FORMS += converterdialog.ui \
    preseteditor.ui

RESOURCES = translations/translations.qrc presets.qrc

LIBS += $$QMMPUI_LIB
PKGCONFIG += taglib

unix{
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
