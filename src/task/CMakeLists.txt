set (LIBRARY ${PROJECT_NAME}Task)
add_library (${LIBRARY} STATIC "")

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
if (UNIX)
# target_include_directories (${LIBRARY} PUBLIC
#                              $<BUILD_INTERFACE:/usr/include>)
elseif (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../error>)
endif ()
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../task>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../timer>)

############## next target ################
set (source_files
     common_itask.h
     common_itaskcontrol.h
     common_task.cpp
     common_task.h
     common_task.inl
     common_task_base.cpp
     common_task_base.h
     common_task_base.inl
     common_task_common.h
     common_task_defines.h
     common_task_ex.cpp
     common_task_ex.h
     common_task_ex.inl
     common_task_manager.cpp
     common_task_manager.h
     common_task_manager.inl
     common_task_tools.cpp
     common_task_tools.h)

# Include dir for installed targets
set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

#set_target_properties (${PROJECT_NAME}Task PROPERTIES LINKER_LANGUAGE CXX)

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

#add_dependencies (${LIBRARY} ACE)

if (UNIX)
# set_property(TARGET ${LIBRARY} PROPERTY CXX_STANDARD 11)
elseif (WIN32)
 target_link_libraries (${LIBRARY} ${ACE_LIBRARY})
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
         common_itask.h
         common_itaskcontrol.h
         common_task.cpp
         common_task.h
         common_task.inl
         common_task_base.cpp
         common_task_base.h
         common_task_base.inl
         common_task_common.h
         common_task_defines.h
         common_task_ex.h
         common_task_ex.inl
         common_task_manager.h
         common_task_manager.inl
         common_task_tools.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
