include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Output/null

HEADERS += outputnullfactory.h \
           outputnull.h

SOURCES += outputnullfactory.cpp \
           outputnull.cpp

RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/Output
    INSTALLS += target
}
