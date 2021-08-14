project(libqmmpui)

ADD_DEFINITIONS(-DQMMPUI_LIBRARY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

link_directories(${CMAKE_INSTALL_PREFIX}/${LIB_DIR})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../qmmp)

SET(QMMP_DEFAULT_UI "" CACHE STRING "default user interface")
IF(QMMP_DEFAULT_UI)
    SET(libqmmpui_DEFINES ${libqmmpui_DEFINES} QMMP_DEFAULT_UI="${QMMP_DEFAULT_OUTPUT}")
    MESSAGE(STATUS "Default user interface: ${QMMP_DEFAULT_UI}")
ENDIF(QMMP_DEFAULT_UI)


SET(libqmmpui_SRCS
    general.cpp
    uihelper.cpp
    playlistparser.cpp
    commandlinemanager.cpp
    commandlinehandler.cpp
    filedialog.cpp
    qtfiledialog.cpp
    fileloader.cpp
    playstate.cpp
    playlistmodel.cpp
    playlistitem.cpp
    playlistgroup.cpp
    playlisttrack.cpp
    playlistcontainer.cpp
    normalcontainer.cpp
    groupedcontainer.cpp
    mediaplayer.cpp
    detailsdialog.cpp
    tageditor.cpp
    playlistmanager.cpp
    metadataformatter.cpp
    metadataformattermenu.cpp
    templateeditor.cpp
    uiloader.cpp
    jumptotrackdialog.cpp
    configdialog.cpp
    pluginitem.cpp
    aboutdialog.cpp
    qmmpuisettings.cpp
    radioitemdelegate.cpp
    playlistdownloader.cpp
    addurldialog.cpp
    qmmpuiplugincache.cpp
    playlisttask.cpp
    playlistheadermodel.cpp
    columneditor.cpp
    metadatahelper.cpp
    coverviewer.cpp
    covereditor.cpp
    generalfactory.cpp
    cueeditor.cpp
    forms/detailsdialog.ui
    forms/tageditor.ui
    forms/templateeditor.ui
    forms/jumptotrackdialog.ui
    forms/configdialog.ui
    forms/aboutdialog.ui
    forms/addurldialog.ui
    forms/columneditor.ui
    forms/covereditor.ui
    forms/cueeditor.ui
    translations/libqmmpui_locales.qrc
    txt/txt.qrc
    images/qmmpui_images.qrc
)

SET(libqmmpui_DEVEL_HDRS
    commandlinemanager.h
    commandlinehandler.h
    configdialog.h
    detailsdialog.h
    filedialogfactory.h
    filedialog.h
    generalfactory.h
    general.h
    mediaplayer.h
    metadataformatter.h
    metadataformattermenu.h
    playlistdownloader.h
    playlistformat.h
    playlistitem.h
    playlistgroup.h
    playlisttrack.h
    playlistmanager.h
    playlistmodel.h
    playlistparser.h
    qmmpuisettings.h
    templateeditor.h
    uifactory.h
    uihelper.h
    uiloader.h
    playlistheadermodel.h
    qmmpui_export.h
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
SET(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${CMAKE_CURRENT_SOURCE_DIR}/forms)

ADD_LIBRARY(libqmmpui SHARED ${libqmmpui_SRCS})
target_link_libraries(libqmmpui PRIVATE Qt6::Widgets Qt6::Network libqmmp)
add_dependencies(libqmmpui libqmmp)
SET_TARGET_PROPERTIES(libqmmpui PROPERTIES VERSION ${QMMP_VERSION} SOVERSION ${QMMP_SOVERSION} OUTPUT_NAME qmmpui${APP_NAME_SUFFIX})
target_compile_definitions(libqmmpui PRIVATE ${libqmmpui_DEFINES})
install(TARGETS libqmmpui LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})


IF(APP_NAME_SUFFIX)
    install(FILES ${libqmmpui_DEVEL_HDRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qmmp${APP_NAME_SUFFIX}/qmmpui)
    SET(QMMP_INCLUDE_DIR ${CMAKE_INSTALL_FULL_INCLUDEDIR}/qmmp${APP_NAME_SUFFIX})
ELSE()
    install(FILES ${libqmmpui_DEVEL_HDRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qmmpui)
    SET(QMMP_INCLUDE_DIR ${CMAKE_INSTALL_FULL_INCLUDEDIR})
ENDIF()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qmmpui.pc.in ${CMAKE_CURRENT_BINARY_DIR}/qmmpui${APP_NAME_SUFFIX}.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qmmpui${APP_NAME_SUFFIX}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
