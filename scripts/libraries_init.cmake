#if (WIN32)
# set (CMAKE_BUILD_TYPE $<CONFIG> CACHE STRING "build type" FORCE)
#endif (WIN32)

if ($<CONFIG> STREQUAL "Debug" OR # *NOTE*: this does not work
    $<CONFIG> STREQUAL "RelWithDebInfo" OR
    ${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR
    ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
 set (LIB_FILE_SUFFIX "d" CACHE STRING "lib file suffix" FORCE)
else ()
 set (LIB_FILE_SUFFIX "" CACHE STRING "lib file suffix" FORCE)
endif ()

if (WIN32)
 set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll.a .a)
endif (WIN32)
