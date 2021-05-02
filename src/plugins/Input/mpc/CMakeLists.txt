project(libmpc)

INCLUDE(CheckIncludeFileCXX)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libmpc
CHECK_INCLUDE_FILE_CXX(mpc/mpcdec.h MPC_HEADER_NEW_FOUND)
FIND_LIBRARY(MPC_LIB NAME mpcdec PATHS /usr/lib /usr/lib32  /usr/local/lib /usr/local/lib32)

IF(MPC_HEADER_NEW_FOUND AND (NOT MPC_LIB MATCHES "^.*-NOTFOUND"))
SET(MPC_FOUND TRUE CACHE INTERNAL "mpc")
MESSAGE(STATUS "Found MusePack: ${MPC_LIB}")
ENDIF(MPC_HEADER_NEW_FOUND AND (NOT MPC_LIB MATCHES "^.*-NOTFOUND"))

include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})

SET(libmpc_SRCS
  decoder_mpc.cpp
  decodermpcfactory.cpp
  mpcmetadatamodel.cpp
  translations/translations.qrc
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(MPC_FOUND)
ADD_LIBRARY(mpc MODULE ${libmpc_SRCS})
add_dependencies(mpc libqmmp)
target_link_libraries(mpc PRIVATE Qt6::Widgets libqmmp ${MPC_LIB} ${TAGLIB_LDFLAGS})
install(TARGETS mpc DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(MPC_FOUND)
