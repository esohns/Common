include (ACEXML)

set (LIBXML2_SUPPORT_DEFAULT ON)
if (UNIX)
 find_package (LibXml2 MODULE)
 if (LibXml2_FOUND)
  set (LIBXML2_FOUND TRUE)
  set (LIBXML2_INCLUDE_DIRS "${LibXml2_INCLUDE_DIRS}")
  set (LIBXML2_LIBRARIES "${LibXml2_LIBRARIES}")
 endif (LibXml2_FOUND)
elseif (WIN32)
# set (MSXML_FOUND ON)
 set (MSXML_SUPPORT ON)
 add_definitions (-DMSXML_SUPPORT)

 # *TODO*: repair pkgconfig support on this platform
 if (VCPKG_SUPPORT)
  find_package (LibXml2 MODULE)
  find_path (LIBICONV_INCLUDE_DIR NAMES iconv.h
             HINTS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}"
             PATH_SUFFIXES include)
  if (LibXml2_FOUND)
   set (LIBXML2_FOUND TRUE)
   set (LIBXML2_INCLUDE_DIRS "${LibXml2_INCLUDE_DIRS}")
   set (LIBXML2_LIBRARIES "${LibXml2_LIBRARIES}")
   if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
       CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set (ImageMagick_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/debug/bin")
   else ()
    set (ImageMagick_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin")
   endif ()
  endif (LibXml2_FOUND)
 else ()
  find_path (LIBICONV_INCLUDE_DIR NAMES iconv.h
             HINTS "$ENV{LIB_ROOT}/libiconv"
             PATH_SUFFIXES include)
  find_path (LIBXML2_INCLUDE_DIR NAMES libxml/xpath.h
             HINTS "$ENV{LIB_ROOT}/libxml2"
             PATH_SUFFIXES include)
  find_library (LIBICONV_LIBRARY NAMES iconv.lib
                HINTS "$ENV{LIB_ROOT}/libiconv"
                PATH_SUFFIXES lib)
  find_library (LIBXML2_LIBRARY NAMES libxml2.lib
                HINTS "$ENV{LIB_ROOT}/libxml2"
                PATH_SUFFIXES lib)
  if (LIBICONV_LIBRARY AND LIBXML2_LIBRARY)
   set (LIBXML2_FOUND TRUE)
   set (LIBXML2_INCLUDE_DIRS "${LIBICONV_INCLUDE_DIR};${LIBXML2_INCLUDE_DIR}")
   set (LIBXML2_LIBRARIES "${LIBICONV_LIBRARY};${LIBXML2_LIBRARY}")
   set (LIBXML2_RUNTIME_LIBDIRS "$ENV{LIB_ROOT}/libiconv/bin;$ENV{LIB_ROOT}/libxml2/bin")
  endif (LIBICONV_LIBRARY AND LIBXML2_LIBRARY)
 endif (VCPKG_SUPPORT)
endif ()
if (LIBXML2_FOUND)
 option (LIBXML2_SUPPORT "enable libxml2 support" ${LIBXML2_SUPPORT_DEFAULT})
 if (LIBXML2_SUPPORT)
  add_definitions (-DLIBXML2_SUPPORT)
 endif (LIBXML2_SUPPORT)
endif (LIBXML2_FOUND)

if (EXISTS ${ACEXML_LIBRARY} AND EXISTS ${ACEXML_PARSER_LIBRARY})
 set (ACEXML_SUPPORT ON)
# add_definitions (-DACEXML_HAS_DLL)
 add_definitions (-DACEXML_SUPPORT)
endif (EXISTS ${ACEXML_LIBRARY} AND EXISTS ${ACEXML_PARSER_LIBRARY})
