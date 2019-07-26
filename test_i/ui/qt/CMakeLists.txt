#cmake_minimum_required(VERSION 3.1.0)

#project(test)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/ui/qt)

# Populate a CMake variable with the sources
set(qt_ui_SRCS
    mainwindow.ui
    mainwindow.cpp
    mainwindow.h
    test_u.cpp
)
set(qt_ui_2_SRCS
    mainwindow.ui
    mainwindow.cpp
    mainwindow.h
    mainwindow_2.cpp
    mainwindow_2.h
    mainwindow_2.inl
    test_u_2.cpp
)

# Tell CMake to create the helloworld executable
add_executable(qt_ui ${qt_ui_SRCS})
add_executable(qt_ui_2 ${qt_ui_2_SRCS})

# Use the Widgets module from Qt 5
target_link_libraries(qt_ui Qt5::Widgets)
target_link_libraries(qt_ui_2 Qt5::Widgets)

target_link_libraries (qt_ui_2 ${ACE_LIBRARY})

target_link_libraries(qt_ui_2 CommonUI_Qt)
