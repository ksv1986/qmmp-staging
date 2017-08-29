include(../../plugins.pri)

HEADERS += outputnullfactory.h \
           outputnull.h

SOURCES += outputnullfactory.cpp \
           outputnull.cpp


TARGET=$$PLUGINS_PREFIX/Output/null

RESOURCES = translations/translations.qrc

unix {
    target.path = $$LIB_DIR/qmmp/Output
    INSTALLS += target
    QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libnull.so
}
