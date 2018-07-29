# this one is important
# *NOTE*: see also: https://cmake.org/cmake/help/v3.8/manual/cmake-toolchains.7.html

if (UNIX)
 set (CMAKE_SYSTEM_NAME Linux)
 # this one not so much
 set (CMAKE_SYSTEM_VERSION 1)
elseif (WIN32)
 # set target platform/toolset (i.e. SDK-) versions
 if (${TARGET_PLATFORM} MATCHES "Host System" OR "XP" OR "Vista" OR "7" OR "8" OR "10")
  set (SYSTEM_NAME Windows)
  if (${TARGET_PLATFORM} MATCHES "Host System")
# *BUG*: CMAKE_HOST_SYSTEM_VERSION not set on Win32
   if (NOT CMAKE_HOST_SYSTEM_VERSION)
    message (STATUS "CMAKE_HOST_SYSTEM_VERSION not set, falling back")
    set (SYSTEM_VERSION 5.1)
   else ()
    set (SYSTEM_VERSION ${CMAKE_HOST_SYSTEM_VERSION})
   endif (NOT CMAKE_HOST_SYSTEM_VERSION)
  elseif (${TARGET_PLATFORM} MATCHES "XP")
   set (SYSTEM_VERSION 5.2)
  elseif (${TARGET_PLATFORM} MATCHES "Vista")
   set (SYSTEM_VERSION 6.0)
  elseif (${TARGET_PLATFORM} MATCHES "7")
   set (SYSTEM_VERSION 6.1)
  elseif (${TARGET_PLATFORM} MATCHES "8")
   set (SYSTEM_VERSION 6.2)
  elseif (${TARGET_PLATFORM} MATCHES "10")
   set (SYSTEM_VERSION 10.0) # *NOTE*: most current of the installed SDK(s)
#  elseif (${TARGET_PLATFORM} MATCHES "10")
#   set (SYSTEM_VERSION 10.0.10586.0)
  else ()
   message (FATAL_ERROR "target platform \(was: ${TARGET_PLATFORM}\) not supported")
  endif ()
 elseif (${TARGET_PLATFORM} MATCHES "CE")
  set (SYSTEM_NAME WindowsCE)
  set (SYSTEM_VERSION 8.0) # Windows Embedded Compact 2013
#  set (CMAKE_SYSTEM_PROCESSOR arm)
#  set (CMAKE_GENERATOR_TOOLSET CE800) # Can be omitted for 8.0
#  set (CMAKE_GENERATOR_PLATFORM SDK_AM335X_SK_WEC2013_V310)
 elseif (${TARGET_PLATFORM} MATCHES "Phone")
  set (SYSTEM_NAME WindowsPhone)
  set (SYSTEM_VERSION 8.1)
 elseif (${TARGET_PLATFORM} MATCHES "Android")
  set (SYSTEM_NAME Android)
#  set (CMAKE_ANDROID_NDK "C:\") # or, set CMAKE_ANDROID_STANDALONE_TOOLCHAIN
#  set (CMAKE_SYSTEM_VERSION 21) # API level
#  set (CMAKE_ANDROID_ARCH_ABI arm64-v8a)
#  set (CMAKE_ANDROID_NDK /path/to/android-ndk)
#  set (CMAKE_ANDROID_STL_TYPE gnustl_static)
 elseif (${TARGET_PLATFORM} MATCHES "Store")
  set (SYSTEM_NAME WindowsStore)
  set (SYSTEM_VERSION 8.1)
 endif ()
 # *NOTE*: default is the same as CMAKE_HOST_SYSTEM_NAME, i.e. the build will
 #         target the host system
 set (CMAKE_SYSTEM_NAME ${SYSTEM_NAME})
 message (STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
 # *NOTE*: iff CMAKE_SYSTEM_NAME is the default, default is the same as
 #         CMAKE_HOST_SYSTEM_VERSION, i.e. the build will target the host system
 set (CMAKE_SYSTEM_VERSION ${SYSTEM_VERSION})
 message (STATUS "CMAKE_SYSTEM_VERSION: ${CMAKE_SYSTEM_VERSION}")

 include (${CMAKE_CURRENT_SOURCE_DIR}/../libCommon/scripts/Macros.cmake)
 get_WIN32_WINNT (version)
 add_definitions("/DWINVER=${version}" "/D_WIN32_WINNT=${version}")
endif ()

