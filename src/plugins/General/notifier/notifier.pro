include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/notifier

HEADERS += notifierfactory.h \
           notifier.h \
           popupwidget.h \
           settingsdialog.h

SOURCES += notifierfactory.cpp \
           notifier.cpp \
           popupwidget.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = notifier_images.qrc \
            translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
  target.path = $$LIB_DIR/qmmp/General
  INSTALLS += target
  PKGCONFIG += x11
  QT += x11extras
  DEFINES += X11_FOUND
}
