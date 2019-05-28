add_library (${PROJECT_NAME}Log STATIC "")

if (UNIX)
# include_directories (${PKG_GTK_GLADE_INCLUDE_DIRS})
 target_include_directories (${PROJECT_NAME}Log PUBLIC
#                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE>)
#                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
                             $<BUILD_INTERFACE:/usr/local/src/ACE_wrappers>)
elseif (WIN32)
#*TODO*: use pkg-config instead
 target_include_directories (${PROJECT_NAME}Log PUBLIC
                             $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
endif ()
target_include_directories (${PROJECT_NAME}Log PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${PROJECT_NAME}Log PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)

############## next target ################
set (source_files
     common_log_common.h
     common_log_tools.cpp
     common_log_tools.h
     common_logger.cpp
     common_logger.h
     common_logger.inl)

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
target_sources (${PROJECT_NAME}Log
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${absolute_source_files})

if (WIN32)
## set (CMAKE_GENERATOR_TOOLSET "LLVM-vs2014" CACHE STRING "Platform Toolset" FORCE)
# set (CMAKE_VS_PLATFORM_TOOLSET "LLVM-vs2014" CACHE STRING "Platform Toolset" FORCE)
 if (CMAKE_VS_PLATFORM_TOOLSET STREQUAL LLVM-vs2014)
  add_compile_options (-Wno-c++11-narrowing)
 endif ()
endif ()

#*TODO*: use pkg-config instead
if (WIN32)
# target_link_libraries (${PROJECT_NAME}Log ${COMMON_TARGET_NAME})
endif ()

# *WARNING*: linking libglade-2.dll(.a) requires "/SAFESEH:NO""
if (WIN32)
# set_target_properties (${PROJECT_NAME}Log PROPERTIES LINK_FLAGS /SAFESEH:NO)
endif ()
set_target_properties (${PROJECT_NAME}Log PROPERTIES DEFINE_SYMBOL COMMON_ERR_BUILD_DLL)
set_target_properties (${PROJECT_NAME}Log PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
set_target_properties (${PROJECT_NAME}Error PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}Log)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}Log
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_log_common.h
         common_log_tools.h
         common_logger.h
         common_logger.inl
         DESTINATION include)
install (TARGETS ${PROJECT_NAME}Log
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
