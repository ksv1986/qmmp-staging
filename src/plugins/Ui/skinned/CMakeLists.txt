project(skinned)

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libskinned_DEFINES APP_NAME_SUFFIX="${APP_NAME_SUFFIX}")

# X11
include_directories(${X11_INCLUDE_DIRS})
link_directories(${X11_LIBRARY_DIRS})
ADD_DEFINITIONS(${X11_CFLAGS})

SET(skinned_SRCS
    fft.c
    balancebar.cpp
    button.cpp
    skinnedsettings.cpp
    display.cpp
    dock.cpp
    eqgraph.cpp
    eqpreset.cpp
    eqslider.cpp
    eqtitlebar.cpp
    eqwidget.cpp
    keyboardmanager.cpp
    listwidget.cpp
    listwidgetdrawer.cpp
    mainvisual.cpp
    mainwindow.cpp
    monostereo.cpp
    number.cpp
    pixmapwidget.cpp
    playlistcontrol.cpp
    playlist.cpp
    playlistslider.cpp
    playlisttitlebar.cpp
    playstatus.cpp
    positionbar.cpp
    preseteditor.cpp
    shadedbar.cpp
    shadedvisual.cpp
    skin.cpp
    skinreader.cpp
    symboldisplay.cpp
    textscroller.cpp
    timeindicator.cpp
    titlebarcontrol.cpp
    titlebar.cpp
    togglebutton.cpp
    visualmenu.cpp
    volumebar.cpp
    cursorimage.cpp
    playlistbrowser.cpp
    playlistselector.cpp
    popupwidget.cpp
    popupsettings.cpp
    windowsystem.cpp
    actionmanager.cpp
    shortcutitem.cpp
    shortcutdialog.cpp
    skinnedfactory.cpp
    hotkeyeditor.cpp
    playlistheader.cpp
    horizontalslider.cpp
)

SET(skinned_HDRS
    eqpreset.h
    fft.h
    inlines.h
    keyboardmanager.h
    listwidgetdrawer.h
    cursorimage.h
    windowsystem.h
    shortcutitem.h
)

SET(skinned_RCCS glare/glare.qrc resources/resources.qrc translations/translations.qrc)

QT5_ADD_RESOURCES(skinned_RCC_SRCS ${skinned_RCCS})

# user interface

SET(skinned_UIS
    forms/skinnedsettings.ui
    forms/preseteditor.ui
    forms/playlistbrowser.ui
    forms/popupsettings.ui
    forms/shortcutdialog.ui
    forms/hotkeyeditor.ui
)

QT5_WRAP_UI(skinned_UIS_H ${skinned_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(X11_FOUND)
ADD_LIBRARY(skinned MODULE ${skinned_SRCS} ${skinned_UIS_H} ${skinned_RCC_SRCS} ${skinned_HDRS})
SET_TARGET_PROPERTIES(skinned PROPERTIES COMPILE_DEFINITIONS "${libskinned_DEFINES}")
add_dependencies(skinned libqmmpui libqmmp)
target_link_libraries(skinned Qt5::Widgets Qt5::X11Extras libqmmpui libqmmp ${X11_LDFLAGS})
install(TARGETS skinned DESTINATION ${PLUGIN_DIR}/Ui)
install(FILES DESTINATION share/qmmp${APP_NAME_SUFFIX}/images)
install(FILES kwin.sh DESTINATION share/qmmp${APP_NAME_SUFFIX}/scripts)
ENDIF(X11_FOUND)
