include(../../qmmp.pri)

TEMPLATE = app

unix:TARGET = ../../bin/qmmp$$APP_NAME_SUFFIX
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
    LIBS += -lqmmp$$APP_NAME_SUFFIX -lqmmpui$$APP_NAME_SUFFIX
    QMAKE_LIBDIR += ../../lib
    target.path = $$BIN_DIR
    desktop.files = qmmp$${APP_NAME_SUFFIX}.desktop \
        qmmp-enqueue$${APP_NAME_SUFFIX}.desktop \
        qmmp-dir$${APP_NAME_SUFFIX}.desktop
    desktop.path = $$DATA_DIR/applications
    icon16.files = images/16x16/qmmp$${APP_NAME_SUFFIX}.png
    icon32.files = images/32x32/qmmp$${APP_NAME_SUFFIX}.png
    icon48.files = images/48x48/qmmp$${APP_NAME_SUFFIX}.png
    icon56.files = images/56x56/qmmp$${APP_NAME_SUFFIX}.png
    icon64.files = images/64x64/qmmp$${APP_NAME_SUFFIX}.png
    icon128.files = images/128x128/qmmp$${APP_NAME_SUFFIX}.png
    icon256.files = images/256x256/qmmp$${APP_NAME_SUFFIX}.png
    icon_svg.files = images/scalable/qmmp$${APP_NAME_SUFFIX}.svgz \
        images/scalable/qmmp-simple$${APP_NAME_SUFFIX}.svgz
    appdata.files = appdata/qmmp$${APP_NAME_SUFFIX}.appdata.xml
    icon16.path = $$DATA_DIR/icons/hicolor/16x16/apps
    icon32.path = $$DATA_DIR/icons/hicolor/32x32/apps
    icon48.path = $$DATA_DIR/icons/hicolor/48x48/apps
    icon56.path = $$DATA_DIR/icons/hicolor/56x56/apps
    icon64.path = $$DATA_DIR/icons/hicolor/64x64/apps
    icon128.path = $$DATA_DIR/icons/hicolor/128x128/apps
    icon256.path = $$DATA_DIR/icons/hicolor/256x256/apps
    icon_svg.path = $$DATA_DIR/icons/hicolor/scalable/apps
    appdata.path = $$DATA_DIR/metainfo
    INSTALLS += desktop \
        target \
        icon16 \
        icon32 \
        icon48 \
        icon56 \
        icon64 \
        icon128 \
        icon256 \
        icon_svg \
        appdata
}

win32 {
    LIBS += -lqmmp1 -lqmmpui1
    QMAKE_LIBDIR += ../../bin
    RC_FILE = qmmp.rc
}
