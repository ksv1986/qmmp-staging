include(../../plugins.pri)


TARGET =$$PLUGINS_PREFIX/General/notifier
unix:QMAKE_CLEAN =$$PLUGINS_PREFIX/General/libnotifier.so

LIBS += -lqmmpui

RESOURCES = notifier_images.qrc \
            translations/translations.qrc

unix {
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

