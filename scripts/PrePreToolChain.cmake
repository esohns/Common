#include(CMakeForceCompiler) #Disables checks

# this one is important
if (UNIX)
 set (CMAKE_SYSTEM_NAME Linux)
elseif (WIN32)
 set (CMAKE_SYSTEM_NAME Windows)
endif ()
# this one not so much
set (CMAKE_SYSTEM_VERSION 11)

if (WIN32)
 # when (how) is this set by cmake ?
 set (CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE "x64")
endif (WIN32)
