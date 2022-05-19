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
    settingsdialog.ui
    hotkeydialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(X11_FOUND AND XCB_H_FOUND)
set_property(SOURCE hotkeymanager_x11.cpp PROPERTY SKIP_AUTOMOC ON)
ADD_LIBRARY(hotkey MODULE ${libhotkey_SRCS})
add_dependencies(hotkey libqmmpui)
target_link_libraries(hotkey PRIVATE Qt6::Widgets libqmmpui libqmmp ${X11_LDFLAGS})
install(TARGETS hotkey DESTINATION ${PLUGIN_DIR}/General)
ENDIF(X11_FOUND AND XCB_H_FOUND)
