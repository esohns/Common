set (GETTEXT_SUPPORT_DEFAULT ON)
if (UNIX)
 find_package (Gettext)
 if (Gettext_FOUND)
  message (STATUS "found gettext")
  set (GETTEXT_FOUND TRUE)
  set (GETTEXT_INCLUDE_DIRS "${Gettext_INCLUDE_DIRS}")
  set (GETTEXT_LIBRARIES "${Gettext_LIBRARIES}")
 endif (Gettext_FOUND)

 find_package (Intl)
 if (Intl_FOUND)
  message (STATUS "found libintl")
  set (LIBINTL_FOUND TRUE)
  set (LIBINTL_INCLUDE_DIRS "${Intl_INCLUDE_DIRS}")
  set (LIBINTL_LIBRARIES "${Intl_LIBRARIES}")
 endif (Intl_FOUND)
# pkg_check_modules (PKG_GETTEXT gettext)
elseif (WIN32)
 if (VCPKG_USE)
  find_package (Gettext)
  if (Gettext_FOUND)
   message (STATUS "found gettext")
   set (GETTEXT_FOUND TRUE)
   find_path (LIBINTL_INCLUDE_DIR NAMES libintl.h
              HINTS ${VCPKG_INCLUDE_DIR}
              DOC "searching for libintl.h"
              NO_DEFAULT_PATH)
   set (LIBINTL_INCLUDE_DIRS ${LIBINTL_INCLUDE_DIR})
   set (LIBINTL_LIBRARIES "${VCPKG_LIB_DIR}/libintl.lib")
   set (LIBINTL_LIB_DIR "${VCPKG_BIN_DIR}")
  endif (Gettext_FOUND)

  find_package (Intl)
  if (Intl_FOUND)
   message (STATUS "found libintl")
   set (LIBINTL_FOUND TRUE)
   find_path (LIBINTL_INCLUDE_DIR NAMES libintl.h
              HINTS ${VCPKG_INCLUDE_DIR}
              DOC "searching for libintl.h"
              NO_DEFAULT_PATH)
   set (LIBINTL_INCLUDE_DIRS ${LIBINTL_INCLUDE_DIR})
   set (LIBINTL_LIBRARIES "${VCPKG_LIB_DIR}/libintl.lib")
   set (LIBINTL_LIB_DIR "${VCPKG_BIN_DIR}")
  endif (Intl_FOUND)
 endif (VCPKG_USE)
 if (NOT GETTEXT_FOUND)
  find_path (LIBINTL_INCLUDE_DIR NAMES libintl.h
             PATHS "$ENV{LIB_ROOT}/gettext"
             PATH_SUFFIXES include
             DOC "searching for libintl.h"
             NO_DEFAULT_PATH)
  set (LIBINTL_LIB "intl.dll.lib")
  find_library (LIBINTL_LIBRARY NAMES ${LIBINTL_LIB}
                PATHS "$ENV{LIB_ROOT}/gettext"
                PATH_SUFFIXES lib
                NO_DEFAULT_PATH)
  if (LIBINTL_INCLUDE_DIR AND LIBINTL_LIBRARY)
   message (STATUS "found gettext")
   set (GETTEXT_FOUND TRUE)
   set (LIBINTL_FOUND TRUE)
   set (LIBINTL_INCLUDE_DIRS "${LIBINTL_INCLUDE_DIR}")
   set (LIBINTL_LIBRARIES "${LIBINTL_LIBRARY}")
   set (LIBINTL_LIB_DIR "$ENV{LIB_ROOT}/gettext/bin")
  else ()
   message (WARNING "could not find gettext, continuing")
  endif (LIBINTL_INCLUDE_DIR AND LIBINTL_LIBRARY)
 endif (NOT GETTEXT_FOUND)
endif ()
if (GETTEXT_FOUND AND LIBINTL_FOUND)
 option (GETTEXT_SUPPORT "enable gettext support" ${GETTEXT_SUPPORT_DEFAULT})
 if (GETTEXT_SUPPORT)
  add_definitions (-DGETTEXT_SUPPORT)
 endif (GETTEXT_SUPPORT)
endif (GETTEXT_FOUND AND LIBINTL_FOUND)
