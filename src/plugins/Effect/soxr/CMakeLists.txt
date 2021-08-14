project(libsoxr)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

pkg_check_modules(SOXR soxr)

include_directories(${SOXR_INCLUDE_DIRS})
link_directories(${SOXR_LIBRARY_DIRS})
ADD_DEFINITIONS(${SOXR_CFLAGS})

SET(libsoxr_SRCS
    soxresampler.cpp
    settingsdialog.cpp
    effectsoxrfactory.cpp
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(SOXR_FOUND)
ADD_LIBRARY(soxr MODULE ${libsoxr_SRCS})
add_dependencies(soxr libqmmp)
target_link_libraries(soxr PRIVATE Qt6::Widgets libqmmp ${SOXR_LDFLAGS})
install(TARGETS soxr DESTINATION ${PLUGIN_DIR}/Effect)
ENDIF(SOXR_FOUND)
