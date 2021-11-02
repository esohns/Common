include (SetUIFramework)

#find_package (Qt5Widgets CONFIG REQUIRED)
#message (STATUS "Qt5Widgets_DEFINITIONS: ${Qt5Widgets_DEFINITIONS}")
#message (STATUS "Qt5Widgets_INCLUDE_DIRS: ${Qt5Widgets_INCLUDE_DIRS}")
#message (STATUS "Qt5Widgets_EXECUTABLE_COMPILE_FLAGS: ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

add_library (${PROJECT_NAME}UI_Qt STATIC "")

if (UNIX)
 target_compile_options (${PROJECT_NAME}UI_Qt PRIVATE -fPIC)
endif (UNIX)

target_include_directories (${PROJECT_NAME}UI_Qt PUBLIC
                            "$<BUILD_INTERFACE:${Qt5Core_INCLUDE_DIRS}>")
target_include_directories (${PROJECT_NAME}UI_Qt PUBLIC
                            "$<BUILD_INTERFACE:${Qt5Widgets_INCLUDE_DIRS}>")

target_include_directories (${PROJECT_NAME}UI_Qt PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../../..>)
target_include_directories (${PROJECT_NAME}UI_Qt PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${PROJECT_NAME}UI_Qt PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
target_include_directories (${PROJECT_NAME}UI_Qt PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../image>)
target_include_directories (${PROJECT_NAME}UI_Qt PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../log>)

############## next target ################
set (source_files
     common_ui_qt_application.cpp
     common_ui_qt_application.h
     common_ui_qt_application.inl
     common_ui_qt_common.h
     common_ui_qt_defines.h
     common_ui_qt_iapplication.h
     common_ui_qt_tools.cpp
     common_ui_qt_tools.h
)

unset (absolute_source_files)
foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
 #       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${PROJECT_NAME}UI_Qt
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

#add_dependencies (${PROJECT_NAME}UI_Qt Common)

#message (STATUS "QT_LIBRARIES: ${QT_LIBRARIES}")
target_link_libraries (${PROJECT_NAME}UI_Qt ${QT_LIBRARIES})
if (UNIX)
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_link_libraries (${PROJECT_NAME}UI_Qt Msvcrtd.lib)
 else ()
  target_link_libraries (${PROJECT_NAME}UI_Qt Msvcrt.lib)
 endif ()
endif ()
#target_link_libraries (${PROJECT_NAME}UI_Qt Common)

#set_target_properties (${PROJECT_NAME}UI_Qt PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
#set_target_properties (${PROJECT_NAME}UI_Qt PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
target_precompile_headers (${PROJECT_NAME}UI_Qt
                           PUBLIC stdafx.h)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}UI_Qt
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${PROJECT_NAME}UI_Qt_LIB_HDRS
     common_ui_qt_application.h
     common_ui_qt_application.inl
     common_ui_qt_common.h
     common_ui_qt_defines.h
     common_ui_qt_iapplication.h
     common_ui_qt_tools.h
)
install (FILES
         ${CommonUI_Qt_LIB_HDRS}
         DESTINATION include)

install (TARGETS ${PROJECT_NAME}UI_Qt
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)

