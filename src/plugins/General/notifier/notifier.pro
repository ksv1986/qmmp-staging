include(../../plugins.pri)

CONFIG += warn_on \
plugin

TARGET =$$PLUGINS_PREFIX/General/notifier
unix:QMAKE_CLEAN =$$PLUGINS_PREFIX/General/libnotifier.so


TEMPLATE = lib
unix:LIBS += -lqmmpui -lqmmp
win32:LIBS += -lqmmpui0 -lqmmp0

RESOURCES = notifier_images.qrc \
            translations/translations.qrc

unix {
  CONFIG += link_pkgconfig
  PKGCONFIG += x11
  target.path = $$LIB_DIR/qmmp/General
  INSTALLS += target
}

HEADERS += notifierfactory.h \
 notifier.h \
 popupwidget.h \
 settingsdialog.h
win32:HEADERS += ../../../../src/qmmpui/general.h
SOURCES += notifierfactory.cpp \
 notifier.cpp \
 popupwidget.cpp \
 settingsdialog.cpp

FORMS += settingsdialog.ui

