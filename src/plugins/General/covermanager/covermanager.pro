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

LIBS += -lqmmpui

unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
