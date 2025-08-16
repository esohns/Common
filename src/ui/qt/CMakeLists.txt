#if (WIN32)
# cmake_policy (SET CMP0020 OLD)
#endif (WIN32)

set (LIBRARY ${PROJECT_NAME}UI_Qt)
add_library (${LIBRARY} STATIC "")

if (UNIX)
 target_compile_options (${LIBRARY} PRIVATE -fPIC)
endif (UNIX)

#message (STATUS "QT_INCLUDES_DIRS: ${QT_INCLUDES_DIRS}")
target_include_directories (${LIBRARY} PUBLIC
                            "$<BUILD_INTERFACE:${QT_INCLUDES_DIRS}>")

if (WIN32)
# message (STATUS "ENV{QT5_DIR}: $ENV{QT5_DIR}")
# set (INCLUDE_DIR "$ENV{QT5_DIR}/../../../include")
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:$ENV{LIB_ROOT}/qt/5.15.2/msvc2019_64/include;$ENV{LIB_ROOT}/qt/5.15.2/msvc2019_64/include/QtCore;$ENV{LIB_ROOT}/qt/5.15.2/msvc2019_64/include/QtWidgets;$ENV{LIB_ROOT}/qt/5.15.2/msvc2019_64/include/QtGui>")
endif (WIN32)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../include>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../image>)
target_include_directories (${LIBRARY} PUBLIC
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
target_sources (${LIBRARY}
#                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                PRIVATE ${absolute_source_files})

#add_dependencies (${LIBRARY} Common)

target_link_libraries (${LIBRARY} ${QT_LIBRARIES})
if (UNIX)
elseif (WIN32)
# if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  target_link_libraries (${LIBRARY} Msvcrtd.lib)
# else ()
#  target_link_libraries (${LIBRARY} Msvcrt.lib)
# endif ()
endif ()

target_link_libraries (${LIBRARY} ${ACE_LIBRARY})

target_link_libraries (${LIBRARY} Common)

#set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
#set_target_properties (${LIBRARY} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules/ui")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${LIBRARY}_LIB_HDRS
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

install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
