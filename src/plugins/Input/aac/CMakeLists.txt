project(libaac)

INCLUDE(CheckCXXSourceCompiles)
INCLUDE(CheckIncludeFileCXX)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libfaad

CHECK_INCLUDE_FILE_CXX(neaacdec.h FAAD_FOUND)

IF(FAAD_FOUND)
SET(CMAKE_REQUIRED_LIBRARIES faad)
CHECK_CXX_SOURCE_COMPILES("
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <neaacdec.h>

int main(int argc, char **argv)
{
#ifdef OLD_FAAD_PROTO
unsigned long int srate;
#else
uint32_t srate;
#endif
uint32_t l;
uint8_t *d;
unsigned char chan;
void    *_instance;
        NeAACDecInit2(_instance, d,l, &srate,&chan);
        return 0;
}" FAAD_MODIFIED)
IF(FAAD_MODIFIED)
ADD_DEFINITIONS(-DFAAD_MODIFIED)
ENDIF(FAAD_MODIFIED)
ENDIF(FAAD_FOUND)

include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})

SET(libaac_SRCS
  decoder_aac.cpp
  decoderaacfactory.cpp
  aacfile.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(FAAD_FOUND)
ADD_LIBRARY(aac MODULE ${libaac_SRCS} ${libaac_RCC_SRCS})
add_dependencies(aac libqmmp)
target_link_libraries(aac PRIVATE Qt6::Widgets libqmmp -lfaad ${TAGLIB_LDFLAGS})
install(TARGETS aac DESTINATION ${PLUGIN_DIR}/Input)
ENDIF(FAAD_FOUND)
