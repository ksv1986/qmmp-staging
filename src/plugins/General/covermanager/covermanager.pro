include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/covermanager
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libcovermanager.so


RESOURCES = translations/translations.qrc \
            images/cm_images.qrc
unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
HEADERS += covermanagerfactory.h \
    coverwidget.h \
    covermanager.h

SOURCES += covermanagerfactory.cpp \
    coverwidget.cpp \
    covermanager.cpp

QT += network
LIBS += -lqmmpui
