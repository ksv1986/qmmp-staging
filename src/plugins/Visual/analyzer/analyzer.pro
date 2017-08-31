include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Visual/analyzer

HEADERS += analyzer.h \
           fft.h \
           visualanalyzerfactory.h \
           inlines.h \
           colorwidget.h \
           settingsdialog.h
SOURCES += analyzer.cpp \
           fft.c \
           visualanalyzerfactory.cpp \
           colorwidget.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

unix{
   target.path = $$LIB_DIR/qmmp/Visual
   INSTALLS += target
}

INCLUDEPATH += ./
