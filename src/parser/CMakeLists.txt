set (LIBRARY ${PROJECT_NAME}Parser)
add_library (${LIBRARY} STATIC "")

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/bison>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

############## next target ################
set (source_files
     common_iparser.h
     common_iscanner.h
     common_lexer_base.cpp
     common_lexer_base.h
     common_lexer_base.inl
     common_parser_base.cpp
     common_parser_base.h
     common_parser_base.inl
     common_parser_common.h
     common_parser_cpp_base.cpp
     common_parser_cpp_base.h
     common_parser_cpp_base.inl
     common_parser_defines.h
     common_parser_driver_base.cpp
     common_parser_driver_base.h
     common_parser_driver_base.inl)

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
                PRIVATE ${absolute_source_files})

if (WIN32)
 if (LLVM_USE)
#  target_compile_options (${LIBRARY} PUBLIC -Wno-c++11-narrowing)
 endif (LLVM_USE)
endif (WIN32)

#*TODO*: use pkg-config instead
if (WIN32)
# target_link_libraries (${LIBRARY} ${COMMON_TARGET_NAME})
 target_link_libraries (${LIBRARY} ${ACE_LIBRARY})
endif (WIN32)

#set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_PARSER_BUILD_DLL)
#set_target_properties (${LIBRARY} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_iparser.h
         common_iscanner.h
         common_lexer_base.h
         common_lexer_base.inl
         common_parser_base.h
         common_parser_base.inl
         common_parser_common.h
         common_parser_cpp_base.h
         common_parser_cpp_base.inl
         common_parser_defines.h
         common_parser_driver_base.h
         common_parser_driver_base.inl
         DESTINATION ${INSTALL_INCLUDE_DIR}
         COMPONENT include)
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
         COMPONENT lib)

add_subdirectory (bencoding)
add_subdirectory (m3u)
