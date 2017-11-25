include(../../qmmp.pri)

TEMPLATE = app

unix:TARGET = ../../bin/qmmp
win32:TARGET = ../../../bin/qmmp

QT += network

HEADERS += qmmpstarter.h \
           builtincommandlineoption.h

SOURCES += qmmpstarter.cpp \
           builtincommandlineoption.cpp \
           main.cpp

RESOURCES = images/images.qrc translations/qmmp_locales.qrc

INCLUDEPATH += ../


unix {
    LIBS += -lqmmp -lqmmpui
    QMAKE_LIBDIR += ../../lib
    target.path = /bin
    desktop.files = qmmp.desktop \
        qmmp_enqueue.desktop \
        qmmp_dir.desktop
    desktop.path = /share/applications
    icon16.files = images/16x16/qmmp.png
    icon32.files = images/32x32/qmmp.png
    icon48.files = images/48x48/qmmp.png
    icon_svg.files = images/scalable/qmmp.svgz \
        images/scalable/qmmp-simple.svgz
    icon16.path = /share/icons/hicolor/16x16/apps
    icon32.path = /share/icons/hicolor/32x32/apps
    icon48.path = /share/icons/hicolor/48x48/apps
    icon_svg.path = /share/icons/hicolor/scalable/apps
    INSTALLS += desktop \
        target \
        icon16 \
        icon32 \
        icon48 \
        icon_svg
}

win32 {
    LIBS += -lqmmp0 -lqmmpui0
    QMAKE_LIBDIR += ../../bin
    RC_FILE = qmmp.rc
}
