if ($<CONFIG> STREQUAL "Debug" OR
    $<CONFIG> STREQUAL "RelWithDebInfo" OR
    ${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR
    ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
 set (LIB_FILE_SUFFIX "d" CACHE STRING "lib file suffix" FORCE)
else ()
 set (LIB_FILE_SUFFIX "" CACHE STRING "lib file suffix" FORCE)
endif ()
