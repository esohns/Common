set (LIBRARY ${PROJECT_NAME}Timer)
add_library (${LIBRARY} STATIC "")

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../error>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../task>)

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
target_sources (${LIBRARY}
#                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

#add_dependencies (${LIBRARY} ACE)

#set_target_properties (${LIBRARY} PROPERTIES
#                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

if (UNIX)
 target_link_libraries (${LIBRARY} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
elseif (WIN32)
 target_link_libraries (${LIBRARY} PUBLIC Avrt.lib)

 target_link_libraries (${LIBRARY} PUBLIC ${ACE_LIBRARY})
endif ()

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
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
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
