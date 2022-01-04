include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Ui/qsui

SOURCES += \
    mainwindow.cpp \
    listwidget.cpp \
    visualmenu.cpp \
    positionslider.cpp \
    actionmanager.cpp \
    shortcutdialog.cpp \
    shortcutitem.cpp \
    popupsettings.cpp \
    popupwidget.cpp \
    equalizer.cpp \
    logo.cpp \
    keyboardmanager.cpp \
    aboutqsuidialog.cpp \
    qsuifactory.cpp \
    qsuisettings.cpp \
    fft.c \
    colorwidget.cpp \
    qsuitabwidget.cpp \
    qsuitabbar.cpp \
    eqpreset.cpp \
    filesystembrowser.cpp \
    elidinglabel.cpp \
    coverwidget.cpp \
    playlistbrowser.cpp \
    toolbareditor.cpp \
    listwidgetdrawer.cpp \
    playlistheader.cpp \
    hotkeyeditor.cpp \
    volumeslider.cpp \
    qsuiquicksearch.cpp \
    qsuivisualization.cpp \
    qsuiwaveformseekbar.cpp \
    qsuistatusbar.cpp \
    dockwidgetlist.cpp
HEADERS += mainwindow.h \
    listwidget.h \
    visualmenu.h \
    positionslider.h \
    actionmanager.h \
    shortcutdialog.h \
    shortcutitem.h \
    popupsettings.h \
    popupwidget.h \
    equalizer.h \
    logo.h \
    keyboardmanager.h \
    aboutqsuidialog.h \
    qsuifactory.h \
    qsuisettings.h \
    fft.h \
    inlines.h \
    colorwidget.h \
    qsuitabwidget.h \
    qsuitabbar.h \
    eqpreset.h \
    filesystembrowser.h \
    elidinglabel.h \
    coverwidget.h \
    playlistbrowser.h \
    toolbareditor.h \
    listwidgetdrawer.h \
    playlistheader.h \
    hotkeyeditor.h \
    volumeslider.h \
    qsuiquicksearch.h \
    qsuivisualization.h \
    qsuiwaveformseekbar.h \
    qsuistatusbar.h \
    dockwidgetlist.h

FORMS += forms/mainwindow.ui \
    forms/shortcutdialog.ui \
    forms/popupsettings.ui \
    forms/aboutqsuidialog.ui \
    forms/qsuisettings.ui \
    forms/toolbareditor.ui \
    forms/hotkeyeditor.ui

RESOURCES += translations/translations.qrc resources/qsui_resources.qrc txt/qsui_txt.qrc

LIBS += $$QMMPUI_LIB

unix {
  target.path = $$PLUGIN_DIR/Ui
  INSTALLS += target
}

win32 {
  INCLUDEPATH += ./
}
