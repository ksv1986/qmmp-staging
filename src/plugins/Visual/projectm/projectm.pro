include(../../plugins.pri)

QT += openglwidgets

TARGET = $$PLUGINS_PREFIX/Visual/projectm

HEADERS += projectmwidget.h \
           visualprojectmfactory.h \
           projectmplugin.h \
           projectmwrapper.h
SOURCES += projectmplugin.cpp \
           visualprojectmfactory.cpp \
           projectmwidget.cpp \
           projectmwrapper.cpp

RESOURCES = translations/translations.qrc

#DEFINES += PROJECTM_31

unix {
    PKGCONFIG += libprojectM
    target.path = $$PLUGIN_DIR/Visual
    INSTALLS += target
    LIBS += -L/usr/lib -I/usr/include
    #projectM config path
    PROJECTM_CONFIG_FILES = /usr/share/projectM/config.inp \
                            /usr/local/share/projectM/config.inp
    for(path, PROJECTM_CONFIG_FILES) {
         exists($$path) {
            message ("found projectm configuration: "$$path)
           DEFINES += PROJECTM_CONFIG=\\\"$$path\\\"
        }
    }
}

win32 {
    LIBS += -lprojectM.dll -lopengl32
}
