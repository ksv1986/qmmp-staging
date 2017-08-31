include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/rgscan

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

RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

unix {
    PKGCONFIG += taglib
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
