set (LIBRARY ${PROJECT_NAME}Timer)
add_library (${LIBRARY} STATIC "")

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../task>)
if (UNIX)
elseif (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../error>)
endif ()

############## next target ################
set (source_files
     common_itimer.h
     common_itimerhandler.h
     common_time_common.h
     common_timer.cpp
     common_timer.h
     common_timer_common.h
     common_timer_defines.h
     common_timer_handler.cpp
     common_timer_handler.h
     common_timer_manager.cpp
     common_timer_manager.h
     common_timer_manager.inl
     common_timer_manager_common.h
     common_timer_resetcounterhandler.cpp
     common_timer_resetcounterhandler.h
     common_timer_second_publisher.cpp
     common_timer_second_publisher.h
     common_timer_second_publisher.inl
     common_timer_tools.cpp
     common_timer_tools.h)

# Include dir for installed targets
set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

#set_target_properties (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

unset (absolute_source_files)
foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
#       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${PROJECT_NAME}Timer
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

#add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

#add_dependencies (${PROJECT_NAME}Timer ACE)

#set_target_properties (${PROJECT_NAME}Timer PROPERTIES COMPILE_DEFINITIONS ACE_HAS_DLL)
#set_target_properties (${PROJECT_NAME}Timer PROPERTIES
#                       DEFINE_SYMBOL COMMON_BUILD_DLL)
set_target_properties (${PROJECT_NAME}Timer PROPERTIES
                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
                       SOVERSION 1)

if (UNIX)
 set_property(TARGET ${PROJECT_NAME}Timer PROPERTY CXX_STANDARD 11)
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  target_link_libraries (${PROJECT_NAME}Timer Msvcrtd.lib)
 else ()
#  target_link_libraries (${PROJECT_NAME}Timer Msvcrt.lib)
 endif ()
# target_link_libraries (${PROJECT_NAME}Timer Quartz.lib)
# target_link_libraries (${PROJECT_NAME}Timer Secur32.lib)
# target_link_libraries (${PROJECT_NAME}Timer Userenv.lib)

 target_link_libraries (${PROJECT_NAME}Timer ${ACE_LIBRARY})

# target_link_libraries (${PROJECT_NAME}Timer ${PROJECT_NAME}Error)
endif ()

# add PCH
set_target_properties (${PROJECT_NAME}Timer PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}Timer)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}Timer
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_itimer.h
         common_itimerhandler.h
         common_time_common.h
         common_timer.h
         common_timer_common.h
         common_timer_defines.h
         common_timer_handler.h
         common_timer_manager.h
         common_timer_manager.inl
         common_timer_manager_common.h
         common_timer_resetcounterhandler.h
         common_timer_second_publisher.h
         common_timer_second_publisher.inl
         common_timer_tools.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${PROJECT_NAME}Timer
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
