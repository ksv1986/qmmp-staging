project(libhotkey)

INCLUDE(CheckIncludeFile)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${X11_INCLUDE_DIRS})
link_directories(${X11_LIBRARY_DIRS})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# xcb
CHECK_INCLUDE_FILE(xcb/xcb.h XCB_H_FOUND)

SET(libhotkey_SRCS
    settingsdialog.cpp
    hotkeyfactory.cpp
    hotkeymanager_x11.cpp
    hotkeymanager_win.cpp
    hotkeydialog.cpp
)

SET(libhotkey_RCCS
    translations/translations.qrc
)

QT5_ADD_RESOURCES(libhotkey_RCC_SRCS ${libhotkey_RCCS})

# user interface


SET(libhotkey_UIS
    settingsdialog.ui
    hotkeydialog.ui
)

QT5_WRAP_UI(libhotkey_UIS_H ${libhotkey_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(X11_FOUND AND XCB_H_FOUND)
set_property(SOURCE hotkeymanager_x11.cpp PROPERTY SKIP_AUTOMOC ON)
ADD_LIBRARY(hotkey MODULE ${libhotkey_SRCS} ${libhotkey_UIS_H} ${libhotkey_RCC_SRCS})
add_dependencies(hotkey libqmmpui)
target_link_libraries(hotkey Qt5::Widgets Qt5::X11Extras libqmmpui libqmmp ${X11_LDFLAGS})
install(TARGETS hotkey DESTINATION ${PLUGIN_DIR}/General)
ENDIF(X11_FOUND AND XCB_H_FOUND)
