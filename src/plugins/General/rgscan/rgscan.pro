include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/General/rgscan
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/General/librgscan.so

LIBS += -lqmmpui

RESOURCES = translations/translations.qrc
unix{
  PKGCONFIG += taglib
  target.path = $$LIB_DIR/qmmp/General
  INSTALLS += target
}
HEADERS += rgscanfactory.h \
    rgscanhelper.h \
    rgscandialog.h \
    gain_analysis.h \
    rgscanner.h

SOURCES += rgscanfactory.cpp \
    rgscanhelper.cpp \
    rgscandialog.cpp \
    gain_analysis.c \
    rgscanner.cpp

FORMS += rgscandialog.ui
