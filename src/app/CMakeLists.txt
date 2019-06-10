project(app)

INCLUDE(UsePkgConfig)

ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


# libqmmp & libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

link_directories(${CMAKE_INSTALL_PREFIX}/${LIB_DIR})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../qmmp)

SET(app_SRCS
    builtincommandlineoption.cpp
    main.cpp
    qmmpstarter.cpp
)

SET(app_RCCS images/images.qrc translations/qmmp_locales.qrc)

QT5_ADD_RESOURCES(app_RCC_SRCS ${app_RCCS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(qmmp ${app_SRCS} ${app_RCC_SRCS})
target_link_libraries(qmmp Qt5::Widgets libqmmp libqmmpui)
SET_TARGET_PROPERTIES(qmmp PROPERTIES OUTPUT_NAME qmmp${APP_NAME_SUFFIX})
add_dependencies(qmmp libqmmpui libqmmp)

install(TARGETS qmmp DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES qmmp.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES qmmp${APP_NAME_SUFFIX}.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES images/16x16/qmmp${APP_NAME_SUFFIX}.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps)
install(FILES images/32x32/qmmp${APP_NAME_SUFFIX}.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps)
install(FILES images/48x48/qmmp${APP_NAME_SUFFIX}.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps)
install(FILES images/56x56/qmmp${APP_NAME_SUFFIX}.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/56x56/apps)
install(FILES images/64x64/qmmp${APP_NAME_SUFFIX}.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps)
install(FILES images/128x128/qmmp${APP_NAME_SUFFIX}.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps)
install(FILES images/256x256/qmmp${APP_NAME_SUFFIX}.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps)
install(FILES images/scalable/qmmp${APP_NAME_SUFFIX}.svgz images/scalable/qmmp-simple${APP_NAME_SUFFIX}.svgz DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
install(FILES appdata/qmmp${APP_NAME_SUFFIX}.appdata.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

IF(USE_DIR_ASSOC)
    install(FILES qmmp-enqueue${APP_NAME_SUFFIX}.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES qmmp-dir${APP_NAME_SUFFIX}.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
ENDIF(USE_DIR_ASSOC)
