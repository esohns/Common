add_library (${PROJECT_NAME}Error STATIC "")

if (UNIX)
 target_include_directories (${PROJECT_NAME}Error PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE>)
elseif (WIN32)
 if (DEFINED ENV{ACE_ROOT})
  target_include_directories (${PROJECT_NAME}Error PUBLIC
                              $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
 else ()
  target_include_directories (${PROJECT_NAME}Error PUBLIC
                              $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ATCD/ACE>)
 endif (DEFINED ENV{ACE_ROOT})
endif ()
target_include_directories (${PROJECT_NAME}Error PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)
target_include_directories (${PROJECT_NAME}Error PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${PROJECT_NAME}Error PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../log>)

############## next target ################
set (source_files
     common_error_defines.h
     common_error_exports.h
     common_error_tools.cpp
     common_error_tools.h)
if (WIN32)
 set (source_files
      ${source_files}
      common_error_directx.cpp
      common_error_directx.h)
endif (WIN32)

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
target_sources (${PROJECT_NAME}Error
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
 target_link_libraries (${PROJECT_NAME}Error ${ACE_LIBRARY})
endif (WIN32)

# *WARNING*: linking libglade-2.dll(.a) requires "/SAFESEH:NO""
if (WIN32)
# set_target_properties (${PROJECT_NAME}Error PROPERTIES LINK_FLAGS /SAFESEH:NO)
endif (WIN32)
set_target_properties (${PROJECT_NAME}Error PROPERTIES DEFINE_SYMBOL COMMON_ERR_BUILD_DLL)
set_target_properties (${PROJECT_NAME}Error PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
set_target_properties (${PROJECT_NAME}Error PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}Error)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}Error
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_error_defines.h
         common_error_exports.h
         common_error_tools.h
         DESTINATION include)
if (WIN32)
 install (FILES
          common_error_directx.cpp
          common_error_directx.h
          DESTINATION include)
endif (WIN32)
install (TARGETS ${PROJECT_NAME}Error
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
