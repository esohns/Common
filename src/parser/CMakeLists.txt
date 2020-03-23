set (LIBRARY ${PROJECT_NAME}Parser)
add_library (${LIBRARY} STATIC "")

if (UNIX)
# include_directories (${PKG_GTK_GLADE_INCLUDE_DIRS})
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
#                             $<BUILD_INTERFACE:/usr/local/src/ACE_wrappers>)
elseif (WIN32)
 if (DEFINED ENV{ACE_ROOT})
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
 else ()
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
 endif (DEFINED ENV{ACE_ROOT})
endif ()

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/bison>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
#target_include_directories (${LIBRARY} PUBLIC
#                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../timer>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>)

#if (WIN32)
# add_definitions (-D_USE_MATH_DEFINES)
#endif (WIN32)

############## next target ################
set (source_files
     common_iparser.h
     common_iscanner.h
     common_parser_base.cpp
     common_parser_base.h
     common_parser_base.inl
     common_parser_bencoding_common.h
     common_parser_bencoding_tools.cpp
     common_parser_bencoding_tools.h
     common_parser_common.h
     common_parser_cpp_base.cpp
     common_parser_cpp_base.h
     common_parser_cpp_base.inl
     common_parser_defines.h)

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
# target_link_libraries (${LIBRARY} ${COMMON_TARGET_NAME})
 target_link_libraries (${LIBRARY} ${ACE_LIBRARY})
endif ()

# *WARNING*: linking libglade-2.dll(.a) requires "/SAFESEH:NO""
if (WIN32)
# set_target_properties (${PROJECT_NAME}ERR PROPERTIES LINK_FLAGS /SAFESEH:NO)
endif ()
set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_MATH_BUILD_DLL)
set_target_properties (${LIBRARY} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
set_target_properties (${LIBRARY} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${LIBRARY})

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_iparser.h
         common_iscanner.h
         common_parser_base.h
         common_parser_base.inl
         common_parser_bencoding_common.h
         common_parser_bencoding_tools.h
         common_parser_common.h
         common_parser_cpp_base.h
         common_parser_cpp_base.inl
         common_parser_defines.h
         DESTINATION include)
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)

