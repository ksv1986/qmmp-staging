include(../../qmmp.pri)
INCLUDEPATH += ../../../qmmp ../../../
QMAKE_LIBDIR += ../../../../lib

win32 {
  QMAKE_LIBDIR += ../../../../bin
  PLUGINS_PREFIX=../../../../../bin/plugins
}

unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    PLUGINS_PREFIX=../../../../lib/qmmp
}
