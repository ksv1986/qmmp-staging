project(qsui)

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)


SET(libqsui_SRCS
    actionmanager.cpp
    listwidget.cpp
    mainwindow.cpp
    popupsettings.cpp
    popupwidget.cpp
    positionslider.cpp
    shortcutdialog.cpp
    shortcutitem.cpp
    qsuifactory.cpp
    qsuisettings.cpp
    visualmenu.cpp
    equalizer.cpp
    logo.cpp
    aboutqsuidialog.cpp
    keyboardmanager.cpp
    qsuivisualization.cpp
    fft.c
    colorwidget.cpp
    qsuitabwidget.cpp
    qsuitabbar.cpp
    eqpreset.cpp
    filesystembrowser.cpp
    elidinglabel.cpp
    coverwidget.cpp
    playlistbrowser.cpp
    toolbareditor.cpp
    listwidgetdrawer.cpp
    playlistheader.cpp
    hotkeyeditor.cpp
    volumeslider.cpp
    qsuiquicksearch.cpp
    qsuiwaveformseekbar.cpp
    qsuistatusbar.cpp
    dockwidgetlist.cpp
    forms/mainwindow.ui
    forms/popupsettings.ui
    forms/shortcutdialog.ui
    forms/qsuisettings.ui
    forms/aboutqsuidialog.ui
    forms/toolbareditor.ui
    forms/hotkeyeditor.ui
    translations/translations.qrc
    txt/qsui_txt.qrc
    resources/qsui_resources.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
SET(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${CMAKE_CURRENT_SOURCE_DIR}/forms)

ADD_LIBRARY(qsui MODULE ${libqsui_SRCS})
add_dependencies(qsui libqmmpui libqmmp)
target_link_libraries(qsui PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS qsui DESTINATION ${PLUGIN_DIR}/Ui)
