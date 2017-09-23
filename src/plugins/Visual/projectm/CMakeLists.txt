project(libprojectm)

SET (QT_USE_QTOPENGL TRUE)
INCLUDE(FindQt4)
include(${QT_USE_FILE})

# qt plugin
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# projectM
pkg_search_module(PROJECTM libprojectM)
pkg_search_module(PROJECTM_20 libprojectM>=2.0.0)
ADD_DEFINITIONS(${PROJECTM_CFLAGS})

IF(PROJECTM_20_FOUND)
ADD_DEFINITIONS(-DPROJECTM_20)
ENDIF(PROJECTM_20_FOUND)

find_file(PROJECTM_CONFIG config.inp
          PATHS "/usr/share/projectM/" "/usr/local/share/projectM/")

IF(PROJECTM_CONFIG)
SET(libprojecm_DEFINES PROJECTM_CONFIG=\"${PROJECTM_CONFIG}\")
MESSAGE(STATUS "Found projectM configuration: ${PROJECTM_CONFIG}")
ENDIF(PROJECTM_CONFIG)

SET(libprojectm_SRCS
    projectmwidget.cpp
    projectmplugin.cpp
    visualprojectmfactory.cpp
    projectmwrapper.cpp
)

SET(libprojectm_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libprojectm_RCC_SRCS ${libprojectm_RCCS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
IF(PROJECTM_FOUND)
ADD_LIBRARY(projectm MODULE ${libprojectm_SRCS} ${libprojectm_RCC_SRCS})
SET_TARGET_PROPERTIES(projectm PROPERTIES COMPILE_DEFINITIONS "${libprojectm_DEFINES}")
add_dependencies(projectm qmmp)
target_link_libraries(projectm ${QT_LIBRARIES} libqmmp ${PROJECTM_LDFLAGS} -lGL)
install(TARGETS projectm DESTINATION ${LIB_DIR}/qmmp/Visual)
ENDIF(PROJECTM_FOUND)
