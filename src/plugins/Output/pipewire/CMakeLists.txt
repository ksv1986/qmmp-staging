project(libpipewire)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# pipewire
pkg_check_modules(PIPEWIRE libpipewire-0.3>=0.3.26 libspa-0.2>=0.2)

include_directories(${PIPEWIRE_INCLUDE_DIRS})
link_directories(${PIPEWIRE_LIBRARY_DIRS})
ADD_DEFINITIONS(${PIPEWIRE_CFLAGS})

SET(libpipewire_SRCS
  outputpipewire.cpp
  outputpipewirefactory.cpp
)

SET(libpipewire_HDRS
  outputpipewire.h
)

SET(libpipewire_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libpipewire_RCC_SRCS ${libpipewire_RCCS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(PIPEWIRE_FOUND)
ADD_LIBRARY(pipewire MODULE ${libpipewire_SRCS} ${libpipewire_UIS_H}
  ${libpipewire_RCC_SRCS} ${libpipewire_HDRS})
add_dependencies(pipewire libqmmp)
target_link_libraries(pipewire Qt5::Widgets libqmmp ${PIPEWIRE_LDFLAGS})
install(TARGETS pipewire DESTINATION ${PLUGIN_DIR}/Output)
ENDIF(PIPEWIRE_FOUND)
