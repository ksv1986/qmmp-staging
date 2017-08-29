include(../../plugins.pri)


TARGET =$$PLUGINS_PREFIX/General/converter
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libconverter.so

LIBS += -lqmmpui

PKGCONFIG += taglib

RESOURCES = translations/translations.qrc presets.qrc

unix{
target.path = $$LIB_DIR/qmmp/General
INSTALLS += target
}
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
