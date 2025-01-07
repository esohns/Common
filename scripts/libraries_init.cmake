set (IS_DEBUG FALSE)
if ($<CONFIG> STREQUAL "Debug" OR $<CONFIG> STREQUAL "RelWithDebInfo")
 set (IS_DEBUG TRUE)
endif ($<CONFIG> STREQUAL "Debug" OR $<CONFIG> STREQUAL "RelWithDebInfo")
if (DEFINED CMAKE_BUILD_TYPE)
 if ((${CMAKE_BUILD_TYPE} STREQUAL "Debug") OR (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
  set (IS_DEBUG TRUE)
 endif ((${CMAKE_BUILD_TYPE} STREQUAL "Debug") OR (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
endif (DEFINED CMAKE_BUILD_TYPE)
if (IS_DEBUG)
 set (LIB_FILE_SUFFIX "d" CACHE STRING "lib file suffix" FORCE)
else ()
 set (LIB_FILE_SUFFIX "" CACHE STRING "lib file suffix" FORCE)
endif (IS_DEBUG)

if (WIN32)
 set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll.a .a)
endif (WIN32)
