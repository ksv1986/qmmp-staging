include(../../qmmp.pri)

VERSION = $$QMMP_VERSION
INCLUDEPATH += ../
TEMPLATE = lib
DEFINES += QMMPUI_LIBRARY
QT += network

CONFIG += warn_on \
    shared \
    qt \
    thread

QMAKE_LIBDIR += ../../lib \
    qmmpui

unix {
    TARGET = ../../lib/qmmpui$$APP_NAME_SUFFIX
    LIBS += -L../../lib -lqmmp$$APP_NAME_SUFFIX
    target.path = $$LIB_DIR
}

win32 {
    TARGET = ../../../bin/qmmpui
    LIBS += -L../../bin -lqmmp1 -lole32
    HEADERS += winfileassoc.h \
               winfileassocpage_p.h
    SOURCES += winfileassoc.cpp \
               winfileassocpage.cpp
    FORMS += forms/winfileassocpage.ui
}

HEADERS += general.h \
    generalfactory.h \
    playlistformat.h \
    playlistparser.h \
    commandlinemanager.h \
    filedialog.h \
    filedialogfactory.h \
    qtfiledialog_p.h \
    playlistitem.h \
    playlistmodel.h \
    playstate_p.h \
    fileloader_p.h \
    mediaplayer.h \
    detailsdialog.h \
    tageditor_p.h \
    playlistmanager.h \
    templateeditor.h \
    uifactory.h \
    uiloader.h \
    uihelper.h \
    jumptotrackdialog_p.h \
    configdialog.h \
    pluginitem_p.h \
    aboutdialog_p.h \
    qmmpuisettings.h \
    radioitemdelegate_p.h \
    playlistdownloader.h \
    addurldialog_p.h \
    qmmpuiplugincache_p.h \
    tagupdater_p.h \
    playlistgroup.h \
    playlisttrack.h \
    playlistcontainer_p.h \
    groupedcontainer_p.h \
    normalcontainer_p.h \
    playlisttask_p.h \
    metadataformatter.h \
    columneditor_p.h \
    playlistheadermodel.h \
    metadatahelper_p.h \
    coverviewer_p.h \
    metadataformattermenu.h \
    qmmpui_export.h \
    covereditor_p.h \
    commandlinehandler.h

SOURCES += general.cpp \
    playlistparser.cpp \
    commandlinemanager.cpp \
    filedialog.cpp \
    qtfiledialog.cpp \
    playlistmodel.cpp \
    playstate.cpp \
    playlistitem.cpp \
    fileloader.cpp \
    mediaplayer.cpp \
    detailsdialog.cpp \
    tageditor.cpp \
    playlistmanager.cpp \
    templateeditor.cpp \
    uiloader.cpp \
    uihelper.cpp \
    jumptotrackdialog.cpp \
    configdialog.cpp \
    pluginitem.cpp \
    aboutdialog.cpp \
    qmmpuisettings.cpp \
    radioitemdelegate.cpp \
    playlistdownloader.cpp \
    addurldialog.cpp \
    qmmpuiplugincache.cpp \
    tagupdater.cpp \
    playlistgroup.cpp \
    playlisttrack.cpp \
    groupedcontainer.cpp \
    normalcontainer.cpp \
    playlistcontainer.cpp \
    playlisttask.cpp \
    metadataformatter.cpp \
    columneditor.cpp \
    playlistheadermodel.cpp \
    metadatahelper.cpp \
    coverviewer.cpp \
    metadataformattermenu.cpp \
    covereditor.cpp \
    commandlinehandler.cpp \
    generalfactory.cpp

FORMS += forms/detailsdialog.ui \
    forms/tageditor.ui \
    forms/templateeditor.ui \
    forms/jumptotrackdialog.ui \
    forms/configdialog.ui \
    forms/aboutdialog.ui \
    forms/addurldialog.ui \
    forms/columneditor.ui \
    forms/covereditor.ui

unix:DESTDIR = .
RESOURCES += translations/libqmmpui_locales.qrc \
    images/qmmpui_images.qrc \
    txt/txt.qrc
TRANSLATIONS = translations/libqmmpui_ru.ts \
    translations/libqmmpui_tr.ts \
    translations/libqmmpui_zh_CN.ts \
    translations/libqmmpui_cs.ts \
    translations/libqmmpui_pt_BR.ts \
    translations/libqmmpui_uk_UA.ts \
    translations/libqmmpui_zh_TW.ts \
    translations/libqmmpui_de.ts \
    translations/libqmmpui_it.ts \
    translations/libqmmpui_lt.ts \
    translations/libqmmpui_pl_PL.ts \
    translations/libqmmpui_nl.ts \
    translations/libqmmpui_ja.ts \
    translations/libqmmpui_es.ts \
    translations/libqmmpui_sk.ts \
    translations/libqmmpui_sr_BA.ts \
    translations/libqmmpui_sr_RS.ts
unix {
    devel.files += \
    commandlinehandler.h \
    general.h \
    generalfactory.h \
    playlistformat.h \
    playlistparser.h \
    commandlinemanager.h \
    filedialog.h \
    filedialogfactory.h \
    playlistitem.h \
    playlistmodel.h \
    mediaplayer.h \
    detailsdialog.h \
    playlistmanager.h \
    templateeditor.h \
    uifactory.h \
    uiloader.h \
    uihelper.h \
    configdialog.h \
    qmmpuisettings.h \
    playlistdownloader.h \
    playlistgroup.h \
    playlisttrack.h \
    metadataformatter.h \
    playlistheadermodel.h \
    metadataformattermenu.h \
    qmmpui_export.h

    isEmpty(APP_NAME_SUFFIX) {
        devel.path = $$PREFIX/include/qmmpui
    } else {
        devel.path = $$PREFIX/include/qmmp$${APP_NAME_SUFFIX}/qmmpui
    }
    INSTALLS += target \
        devel
}

unix {
    CONFIG += create_pc create_prl no_install_prl
    QMAKE_PKGCONFIG_NAME = qmmpui$${APP_NAME_SUFFIX}
    QMAKE_PKGCONFIG_DESCRIPTION = qmmp user interface library
    QMAKE_PKGCONFIG_REQUIRES = Qt5Core Qt5Gui Qt5Widgets Qt5Network qmmp$${APP_NAME_SUFFIX}
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig
    QMAKE_PKGCONFIG_PREFIX = $$PREFIX
    QMAKE_PKGCONFIG_LIBDIR = $$target.path
    isEmpty(APP_NAME_SUFFIX) {
        QMAKE_PKGCONFIG_INCDIR = $$PREFIX/include
    } else {
        QMAKE_PKGCONFIG_INCDIR = $$PREFIX/include/qmmp$${APP_NAME_SUFFIX}
    }
}
