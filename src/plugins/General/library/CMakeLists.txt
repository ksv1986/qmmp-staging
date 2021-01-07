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
)

SET(liblibrary_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(liblibrary_RCC_SRCS ${liblibrary_RCCS})

# user interface
SET(liblibrary_UIS
    librarywidget.ui
    settingsdialog.ui
)

QT5_WRAP_UI(liblibrary_UIS_H ${liblibrary_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(library MODULE ${liblibrary_SRCS} ${liblibrary_UIS_H} ${liblibrary_RCC_SRCS})
target_link_libraries(library Qt5::Widgets Qt5::Sql Qt5::Concurrent libqmmpui libqmmp)
install(TARGETS library DESTINATION ${PLUGIN_DIR}/General)
