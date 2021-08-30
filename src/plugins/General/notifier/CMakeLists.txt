project(libnotifier)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libnotifier_SRCS
    settingsdialog.cpp
    notifier.cpp
    notifierfactory.cpp
    popupwidget.cpp
    settingsdialog.ui
    translations/translations.qrc
    notifier_images.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(notifier MODULE ${libnotifier_SRCS} ${libnotifier_UIS_H} ${libnotifier_RCC_SRCS})
add_dependencies(notifier libqmmpui)
IF(X11_FOUND AND Qt6BuildInternals_FOUND)
  include_directories(${X11_INCLUDE_DIRS})
  link_directories(${X11_LIBRARY_DIRS})
  ADD_DEFINITIONS(${X11_CFLAGS})
  ADD_DEFINITIONS(-DX11_FOUND)
  IF(Qt6Widgets_VERSION VERSION_GREATER_EQUAL 6.2.0)
    target_link_libraries(notifier PRIVATE Qt6::Widgets ${X11_LDFLAGS} libqmmpui libqmmp)
  ELSE()
    target_link_libraries(notifier PRIVATE Qt6::Widgets Qt6::GuiPrivate ${X11_LDFLAGS} libqmmpui libqmmp)
  ENDIF()
ELSE(X11_FOUND AND Qt6BuildInternals_FOUND)
  target_link_libraries(notifier PRIVATE Qt6::Widgets libqmmpui libqmmp)
ENDIF(X11_FOUND AND Qt6BuildInternals_FOUND)
install(TARGETS notifier DESTINATION ${PLUGIN_DIR}/General)
