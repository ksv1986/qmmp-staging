project(libprojectm)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# projectM
MESSAGE(STATUS "Found OpenGL implementation: ${Qt5Gui_OPENGL_IMPLEMENTATION}")
#IF(${Qt5Gui_OPENGL_IMPLEMENTATION} STREQUAL GL)
  pkg_search_module(PROJECTM libprojectM>=2.0.0)
  ADD_DEFINITIONS(${PROJECTM_CFLAGS})
#ELSE(${Qt5Gui_OPENGL_IMPLEMENTATION} STREQUAL GL)
#  MESSAGE(WARNING "Unsuppoted OpenGL implementation, projectM disabled")
#ENDIF(${Qt5Gui_OPENGL_IMPLEMENTATION} STREQUAL GL)

pkg_search_module(PROJECTM_31 libprojectM>=3.1.0)

IF(PROJECTM_31_FOUND)
ADD_DEFINITIONS(-DPROJECTM_31)
ENDIF(PROJECTM_31_FOUND)

find_file(PROJECTM_CONFIG config.inp
          PATHS "/usr/share/projectM/" "/usr/local/share/projectM/")

IF(PROJECTM_CONFIG)
SET(libprojectm_DEFINES PROJECTM_CONFIG=\"${PROJECTM_CONFIG}\")
MESSAGE(STATUS "Found projectM configuration: ${PROJECTM_CONFIG}")
ENDIF(PROJECTM_CONFIG)

SET(libprojectm_SRCS
    projectmwidget.cpp
    projectmplugin.cpp
    visualprojectmfactory.cpp
    projectmwrapper.cpp
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
IF(PROJECTM_FOUND)
ADD_LIBRARY(projectm MODULE ${libprojectm_SRCS})
target_compile_definitions(projectm PRIVATE ${libprojectm_DEFINES})
add_dependencies(projectm libqmmp)
target_link_libraries(projectm PRIVATE Qt6::OpenGLWidgets libqmmp ${PROJECTM_LDFLAGS} -lGL)
install(TARGETS projectm DESTINATION ${PLUGIN_DIR}/Visual)
ENDIF(PROJECTM_FOUND)
