project(liblibrary)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(liblibrary_SRCS
    library.cpp
    libraryfactory.cpp
    librarymodel.cpp
    librarywidget.cpp
    settingsdialog.cpp
    librarywidget.ui
    settingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(library MODULE ${liblibrary_SRCS})
target_link_libraries(library PRIVATE Qt6::Widgets Qt6::Sql libqmmpui libqmmp)
install(TARGETS library DESTINATION ${PLUGIN_DIR}/General)
