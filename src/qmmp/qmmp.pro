include(../../qmmp.pri)
HEADERS += \
    buffer.h \
    decoder.h \
    output.h \
    outputfactory.h \
    equ/iir_cfs.h \
    equ/iir_fpu.h \
    equ/iir.h \
    decoderfactory.h \
    soundcore.h \
    visual.h \
    visualfactory.h \
    effect.h \
    effectfactory.h \
    statehandler.h \
    qmmp.h \
    metadatamodel.h \
    tagmodel.h \
    abstractengine.h \
    qmmpaudioengine_p.h \
    audioparameters.h \
    inputsource.h \
    fileinputsource_p.h \
    emptyinputsource_p.h \
    inputsourcefactory.h \
    enginefactory.h \
    metadatamanager.h \
    replaygain_p.h \
    qmmpsettings.h \
    eqsettings.h \
    qmmpevents_p.h \
    volume.h \
    outputwriter_p.h \
    recycler_p.h \
    qmmpplugincache_p.h \
    channelmap.h \
    channelconverter_p.h \
    audioconverter.h \
    dithering_p.h \
    visualbuffer_p.h \
    qmmp_export.h \
    trackinfo.h \
    volumehandler.h \
    cueparser.h
SOURCES += recycler.cpp \
    decoder.cpp \
    output.cpp \
    equ/iir.c \
    equ/iir_cfs.c \
    equ/iir_fpu.c \
    soundcore.cpp \
    visual.cpp \
    effect.cpp \
    statehandler.cpp \
    qmmp.cpp \
    metadatamodel.cpp \
    tagmodel.cpp \
    abstractengine.cpp \
    qmmpaudioengine.cpp \
    audioparameters.cpp \
    inputsource.cpp \
    fileinputsource.cpp \
    emptyinputsource.cpp \
    metadatamanager.cpp \
    replaygain.cpp \
    audioconverter.cpp \
    qmmpsettings.cpp \
    eqsettings.cpp \
    qmmpevents.cpp \
    outputwriter.cpp \
    qmmpplugincache.cpp \
    channelmap.cpp \
    channelconverter.cpp \
    volume.cpp \
    dithering.cpp \
    visualbuffer.cpp \
    trackinfo.cpp \
    volumehandler.cpp \
    cueparser.cpp

unix:TARGET = ../../lib/qmmp$$APP_NAME_SUFFIX
win32:TARGET = ../../../bin/qmmp
CONFIG += shared \
    warn_on \
    qt \
    thread
TEMPLATE = lib
VERSION = $$QMMP_VERSION
DEFINES += QMMP_LIBRARY

unix {
  DEFINES += LIB_DIR=\\\"$$LIB_DIR\\\"
  SVN_REVISION = $$system(./svn_revision.sh)
  !isEmpty(SVN_REVISION) {
    DEFINES += SVN_REVISION=\\\"$$SVN_REVISION\\\"
  }
  DEFINES += QMMP_PLUGIN_DIR=\\\"$$PLUGIN_DIR\\\"
}

win32 {
  isEmpty(QMAKE_SH) {
    SVN_REVISION = $$system(svn_revision.cmd)
  } else {
    SVN_REVISION = $$system(sh svn_revision.sh)
  }
  !isEmpty(SVN_REVISION) {
    DEFINES += SVN_REVISION=\\\"$$SVN_REVISION\\\"
  }
}

unix {
    DEFINES += APP_NAME_SUFFIX=\\\"$$APP_NAME_SUFFIX\\\"
    target.path = $$LIB_DIR
    devel.files += \
        abstractengine.h \
        audioconverter.h \
        audioparameters.h \
        buffer.h \
        channelmap.h \
        decoderfactory.h \
        decoder.h \
        effectfactory.h \
        effect.h \
        enginefactory.h \
        eqsettings.h \
        fileinfo.h \
        inputsourcefactory.h \
        inputsource.h \
        metadatamanager.h \
        metadatamodel.h \
        outputfactory.h \
        output.h \
        qmmp.h \
        qmmpsettings.h \
        soundcore.h \
        statehandler.h \
        tagmodel.h \
        trackinfo.h \
        visualfactory.h \
        visual.h \
        volume.h \
        volumehandler.h \
        cueparser.h \
        qmmp_export.h

    isEmpty(APP_NAME_SUFFIX) {
        devel.path = $$PREFIX/include/qmmp
    } else {
        devel.path = $$PREFIX/include/qmmp$${APP_NAME_SUFFIX}/qmmp
    }
    INSTALLS += target \
        devel
    DESTDIR = .
}
INCLUDEPATH += ./


unix {
    CONFIG += create_pc create_prl no_install_prl
    QMAKE_PKGCONFIG_NAME = qmmp$${APP_NAME_SUFFIX}
    QMAKE_PKGCONFIG_DESCRIPTION = qmmp core library
    QMAKE_PKGCONFIG_REQUIRES = Qt5Core Qt5Gui Qt5Widgets
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig
    QMAKE_PKGCONFIG_PREFIX = $$PREFIX
    QMAKE_PKGCONFIG_LIBDIR = $$target.path
    isEmpty(APP_NAME_SUFFIX) {
        QMAKE_PKGCONFIG_INCDIR = $$PREFIX/include
    } else {
        QMAKE_PKGCONFIG_INCDIR = $$PREFIX/include/qmmp$${APP_NAME_SUFFIX}
    }
}
