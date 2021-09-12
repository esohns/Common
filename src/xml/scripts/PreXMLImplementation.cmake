include (ACEXML)

if (UNIX)
 find_package (LibXml2 MODULE)
elseif (WIN32)
# set (MSXML_FOUND ON)
 set (MSXML_SUPPORT ON)
 add_definitions (-DMSXML_SUPPORT)

 # *TODO*: repair pkgconfig support on this platform
 find_path (LIBICONV_INCLUDE_DIR NAMES iconv.h
            HINTS "$ENV{LIB_ROOT}/libiconv"
            PATH_SUFFIXES include)
 find_path (LIBXML2_INCLUDE_DIR NAMES libxml/xpath.h
            HINTS "$ENV{LIB_ROOT}/libxml2"
            PATH_SUFFIXES include)
 set (LIBXML2_INCLUDE_DIRS "${LIBICONV_INCLUDE_DIR};${LIBXML2_INCLUDE_DIR}")
 find_library (LIBICONV_LIBRARY NAMES libiconv.lib
               HINTS "$ENV{LIB_ROOT}/libiconv"
               PATH_SUFFIXES lib)
 find_library (LIBXML2_LIBRARY NAMES libxml2.lib
               HINTS "$ENV{LIB_ROOT}/libxml2"
               PATH_SUFFIXES lib)
 set (LIBXML2_LIBRARIES "${LIBICONV_LIBRARY};${LIBXML2_LIBRARY}")
 set (LIBXML2_RUNTIME_LIBDIRS "$ENV{LIB_ROOT}/libiconv/bin;$ENV{LIB_ROOT}/libxml2/bin")
 set (LIBXML2_FOUND ON)
endif ()
if (LIBXML2_FOUND)
 set (LIBXML2_SUPPORT ON)
 add_definitions (-DLIBXML2_SUPPORT)
endif (LIBXML2_FOUND)
if (EXISTS ${ACEXML_LIBRARY} AND EXISTS ${ACEXML_PARSER_LIBRARY})
 set (ACEXML_SUPPORT ON)
# add_definitions (-DACEXML_HAS_DLL)
 add_definitions (-DACEXML_SUPPORT)
endif (EXISTS ${ACEXML_LIBRARY} AND EXISTS ${ACEXML_PARSER_LIBRARY})
