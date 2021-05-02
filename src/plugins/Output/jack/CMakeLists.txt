project(libjack)

INCLUDE (CheckCXXSourceCompiles)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libjack and libsoxr
pkg_check_modules(JACK jack>=1.9.8 soxr)

IF(NOT JACK_FOUND)
pkg_check_modules(JACK jack>=0.122.0 soxr)
ENDIF(NOT JACK_FOUND)

include_directories(${JACK_INCLUDE_DIRS})
link_directories(${JACK_LIBRARY_DIRS})
ADD_DEFINITIONS(${JACK_CFLAGS})

SET(libjack_SRCS
  outputjackfactory.cpp
  outputjack.cpp
  bio2jack.c
  translations/translations.qrc
)

IF(JACK_FOUND)
ADD_LIBRARY(jack MODULE ${libjack_SRCS})
add_dependencies(jack libqmmp)
set_target_properties(jack PROPERTIES
                           COMPILE_FLAGS -pthread
                           LINK_FLAGS -pthread
)
target_link_libraries(jack PRIVATE Qt6::Widgets libqmmp ${JACK_LDFLAGS})
install(TARGETS jack DESTINATION ${PLUGIN_DIR}/Output)
ENDIF(JACK_FOUND)

