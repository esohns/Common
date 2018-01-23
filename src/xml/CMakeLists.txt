set (ACE_LIB_FILE libACE.so)
if (UNIX)
# message ("CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}")
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               HINTS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE /usr/local
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib
               DOC "searching for ${ACE_LIB_FILE}"
               NO_DEFAULT_PATH)
# else ()
#  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
#                DOC "searching for ${ACE_LIB_FILE} (system paths)")
# endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_LIB_FILE)
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
else ()
 #message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()
add_definitions (-DACE_HAS_DLL)

set (ACEXML_LIB_FILE libACEXML.so)
if (UNIX)
# message ("CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}")
 find_library (ACEXML_LIBRARY ${ACEXML_LIB_FILE}
               HINTS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE /usr/local
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib
               DOC "searching for ${ACEXML_LIB_FILE}"
               NO_DEFAULT_PATH)
# else ()
#  find_library (ACEXML_LIBRARY ${ACEXML_LIB_FILE}
#                DOC "searching for ${ACEXML_LIB_FILE} (system paths)")
# endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACEXML_LIB_FILE)
 set (ACEXML_LIB_FILE ACEXML${LIB_FILE_SUFFIX}.lib)
 find_library (ACEXML_LIBRARY ${ACEXML_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACEXML_LIB_FILE}")
endif ()
#if (NOT EXISTS ACEXML_LIBRARY)
if (NOT ACEXML_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACEXML_LIB_FILE} (was: \"${ACEXML_LIBRARY}\"), aborting")
else ()
 #message (STATUS "Found ACEXML library \"${ACEXML_LIBRARY}\"")
endif ()
#add_definitions (-DACEXML_HAS_DLL)

if (UNIX)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)
elseif (WIN32)
 include_directories ($ENV{ACE_ROOT})
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/err)
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

############## next target ################
set (${PROJECT_NAME}XML_LIB_SRCS
     common_xml_common.h
     common_xml_defines.h
     common_xml_handler.cpp
     common_xml_handler.h
     common_xml_handler_base.cpp
     common_xml_handler_base.h
     common_xml_handler_base.inl
     common_xml_parser.cpp
     common_xml_parser.h
     common_xml_parser.inl
     common_xml_tools.cpp
     common_xml_tools.h
     stdafx.cpp
     stdafx.h)

#set (LLVM_COMPILER_ID_STRING "Clang")
#string (TOLOWER "${LLVM_COMPILER_ID_STRING}" LLVM_COMPILER_ID_STRING_LOWER)
#string (TOLOWER "${CMAKE_CXX_COMPILER_ID}" CXX_COMPILER_ID_STRING_LOWER)
#if (CXX_COMPILER_ID_STRING_LOWER STREQUAL LLVM_COMPILER_ID_STRING_LOWER)
# add_compile_options (-fPIC)
# add_definitions (-D_GLIBCXX_USE_CXX11_ABI=0)
#endif ()

add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

add_library (${PROJECT_NAME}XML STATIC
             ${CommonXML_LIB_SRCS})

#add_dependencies (${PROJECT_NAME}XML ACE)

#set_target_properties (${PROJECT_NAME}XML PROPERTIES COMPILE_DEFINITIONS ACE_HAS_DLL)
#set_target_properties (${PROJECT_NAME}XML PROPERTIES
#                       DEFINE_SYMBOL COMMON_XML_BUILD_DLL)
set_target_properties (${PROJECT_NAME}XML PROPERTIES
                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
                       SOVERSION 1)

if (UNIX)
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  target_link_libraries (${PROJECT_NAME}Timer Msvcrtd.lib)
 else ()
#  target_link_libraries (${PROJECT_NAME}Timer Msvcrt.lib)
 endif ()
# target_link_libraries (${PROJECT_NAME}Timer Quartz.lib)
# target_link_libraries (${PROJECT_NAME}Timer Secur32.lib)
# target_link_libraries (${PROJECT_NAME}Timer Userenv.lib)

# target_link_libraries (${PROJECT_NAME}Timer ${ACE_LIBRARY})

# target_link_libraries (${PROJECT_NAME}Timer ${PROJECT_NAME}Error)
endif ()

# add PCH
set_target_properties (${PROJECT_NAME}XML PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}XML)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}XML
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_xml_common.h
         common_xml_defines.h
         common_xml_handler.h
         common_xml_handler_base.h
         common_xml_handler_base.inl
         common_xml_parser.h
         common_xml_parser.inl
         common_xml_tools.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${PROJECT_NAME}XML
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
         LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
#         RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
         COMPONENT dev)
