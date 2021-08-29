if (WIN32)
 set (FLEX_EXECUTABLE "H:\\bin\\flex_bison\\win_flex.exe" CACHE PATH "Flex executable")
endif (WIN32)

find_package (FLEX)
if (FLEX_FOUND)
 message (STATUS "Found flex \"${FLEX_EXECUTABLE}\"")
 add_definitions (-DFLEX_SUPPORT)
else ()
 message (WARNING "could not find flex, continuing")
endif (FLEX_FOUND)

if (UNIX)
 if (NOT FLEX_FOUND)
  pkg_check_modules (PKG_FLEX libfl)
  if (PKG_FLEX_FOUND)
   message (STATUS "Found libfl")
   add_definitions (-DFLEX_SUPPORT)
  else ()
   set (FLEX_LIB_FILE libfl.a)
   find_library (FLEX_LIBRARY ${FLEX_LIB_FILE}
                 HINTS /usr/lib64 /usr/lib
                 PATH_SUFFIXES x86_64-linux-gnu
                 DOC "searching for ${FLEX_LIB_FILE} (system paths)")
   if (FLEX_LIBRARY)
    message (STATUS "Found flex library \"${FLEX_LIBRARY}\"")
    add_definitions (-DFLEX_SUPPORT)
   else ()
    message (FATAL_ERROR "could not find ${FLEX_LIB_FILE}, aborting")
   endif (FLEX_LIBRARY)
  endif (PKG_FLEX_FOUND)
 endif (NOT FLEX_FOUND)
endif (UNIX)
