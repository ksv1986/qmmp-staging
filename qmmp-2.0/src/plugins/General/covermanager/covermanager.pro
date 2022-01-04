include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/covermanager

QT += network

HEADERS += covermanagerfactory.h \
    coverwidget.h \
    covermanager.h

SOURCES += covermanagerfactory.cpp \
    coverwidget.cpp \
    covermanager.cpp

RESOURCES = translations/translations.qrc \
            images/cm_images.qrc

LIBS += $$QMMPUI_LIB

unix {
    target.path = $$PLUGIN_DIR/General
    INSTALLS += target
}
