if (UNIX)
 set (ACEXML_LIB_FILE libACEXML.so)
 find_library (ACEXML_LIBRARY ${ACEXML_LIB_FILE}
               PATHS $ENV{ACE_ROOT}/ACEXML ${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE/ACEXML
               PATH_SUFFIXES lib common
               DOC "searching for ${ACEXML_LIB_FILE} (system paths)")

 set (ACEXML_PARSER_LIB_FILE libACEXML_Parser.so)
 find_library (ACEXML_PARSER_LIBRARY ${ACEXML_PARSER_LIB_FILE}
               PATHS $ENV{ACE_ROOT}/ACEXML/parser ${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE/ACEXML/parser
               PATH_SUFFIXES lib parser
               DOC "searching for ${ACEXML_PARSER_LIB_FILE}")
elseif (WIN32)
 set (ACEXML_LIB_FILE ACEXML${LIB_FILE_SUFFIX}.lib) 
 set (ACEXML_PARSER_LIB_FILE ACEXML_Parser${LIB_FILE_SUFFIX}.lib) 
 if (VCPKG_USE AND NOT DEFINED ENV{ACE_ROOT})
#  find_package (ACE
#                COMPONENTS xml)
#  find_path (ACEXML_INCLUDE_DIR ace/ACE.h)
  find_library (ACEXML_LIBRARY ${ACEXML_LIB_FILE})
  find_library (ACEXML_PARSER_LIBRARY ${ACEXML_PARSER_LIB_FILE})
 else ()
  find_library (ACEXML_LIBRARY ${ACEXML_LIB_FILE}
                PATHS $ENV{ACE_ROOT} $ENV{LIB_ROOT}/ACE_TAO/ACE
                PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
                DOC "searching for ${ACEXML_LIB_FILE}"
                NO_DEFAULT_PATH)
  find_library (ACEXML_PARSER_LIBRARY ${ACEXML_PARSER_LIB_FILE}
                PATHS $ENV{ACE_ROOT} $ENV{LIB_ROOT}/ACE_TAO/ACE
                PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
                DOC "searching for ${ACEXML_PARSER_LIB_FILE}"
                NO_DEFAULT_PATH)
 endif (VCPKG_USE AND NOT DEFINED ENV{ACE_ROOT})
endif ()
#if (NOT EXISTS ACEXML_LIBRARY)
if (NOT ACEXML_LIBRARY)
 message (WARNING "could not find ${ACEXML_LIB_FILE}, continuing")
else ()
 message (STATUS "Found ACEXML library \"${ACEXML_LIBRARY}\"")
 add_definitions (-DACEXML_HAS_DLL)
endif (NOT ACEXML_LIBRARY)
if (NOT ACEXML_PARSER_LIBRARY)
 message (WARNING "could not find ${ACEXML_PARSER_LIB_FILE}, continuing")
else ()
 message (STATUS "Found ACEXML_Parser library \"${ACEXML_PARSER_LIBRARY}\"")
endif (NOT ACEXML_PARSER_LIBRARY)
if (EXISTS ${ACEXML_LIBRARY} AND EXISTS ${ACEXML_PARSER_LIBRARY})
 set (ACEXML_SUPPORT ON)
 add_definitions (-DACEXML_SUPPORT)
 set (ACEXML_LIBRARIES ${ACEXML_LIBRARY};${ACEXML_PARSER_LIBRARY})
endif (EXISTS ${ACEXML_LIBRARY} AND EXISTS ${ACEXML_PARSER_LIBRARY})
