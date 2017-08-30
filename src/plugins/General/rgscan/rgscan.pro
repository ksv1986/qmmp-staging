include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/rgscan
CONFIG += link_qmmpui

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

unix {
    PKGCONFIG += taglib
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
