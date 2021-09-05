if (UNIX)
 set (ACEXML_LIB_FILE libACEXML.so)
 find_library (ACE_XML_LIBRARY ${ACEXML_LIB_FILE}
               PATHS $ENV{ACE_ROOT}/ACEXML ${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE/ACEXML
               PATH_SUFFIXES common
               DOC "searching for ${ACEXML_LIB_FILE} (system paths)")
elseif (WIN32)
# *TODO*: this does not work
 set (LIB_FILE_SUFFIX "")
# message (STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
 if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
     CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set (LIB_FILE_SUFFIX "d")
 endif ()

 set (ACEXML_LIB_FILE ACEXML${LIB_FILE_SUFFIX}.lib) 
 if (VCPKG_SUPPORT)
#  find_package (ACE
#                COMPONENTS xml)
#  find_path (ACEXML_INCLUDE_DIR ace/ACE.h)
  find_library (ACEXML_LIBRARY ${ACEXML_LIB_FILE})
 else ()
  find_library (ACEXML_LIBRARY ${ACEXML_LIB_FILE}
                PATHS $ENV{ACE_ROOT} $ENV{LIB_ROOT}/ACE_TAO/ACE
                PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
                DOC "searching for ${ACE_LIB_FILE}"
                REQUIRED
                NO_DEFAULT_PATH)
 endif (VCPKG_SUPPORT)
endif ()
#if (NOT EXISTS ACEXML_LIBRARY)
if (NOT ACEXML_LIBRARY)
 message (WARNING "could not find ${ACEXML_LIB_FILE}, continuing")
else ()
 message (STATUS "Found ACEXML library \"${ACEXML_LIBRARY}\"")
 add_definitions (-DACEXML_HAS_DLL)
endif ()
