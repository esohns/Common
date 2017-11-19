#include (CheckLibraryExists)
#check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
#find_library (HAVE_LIB_ACE libACE)

if (UNIX)
 set (ACE_LIB_FILE libACE.so)
 if (DEFINED ENV{ACE_ROOT})
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                PATHS ENV ACE_ROOT
                PATH_SUFFIXES ace build/linux/ace
                DOC "searching for ${ACE_LIB_FILE}"
                NO_DEFAULT_PATH)
 else ()
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                DOC "searching for ${ACE_LIB_FILE} (system paths)")
 endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
else ()
 message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()

include (FindPkgConfig)
if (UNIX)
 pkg_check_modules (PKG_LIBFFMPEG REQUIRED libavcodec libavformat libswscale)
endif ()

if (UNIX)
 include_directories (${PKG_LIBFFMPEG_INCLUDE_DIRS})
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE)
elseif (WIN32)
#*TODO*: use pkg-config instead
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libpng-1.6.25)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ffmpeg)

 include_directories ($ENV{ACE_ROOT})
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

############## next target ################
set (${PROJECT_NAME}IMAGE_LIB_SRCS
     common_image_exports.h
     common_image_tools.cpp
     common_image_tools.h
     stdafx.cpp
     stdafx.h)

add_library (${PROJECT_NAME}Image STATIC
             ${CommonIMAGE_LIB_SRCS})

if (WIN32)
## set (CMAKE_GENERATOR_TOOLSET "LLVM-vs2014" CACHE STRING "Platform Toolset" FORCE) 
# set (CMAKE_VS_PLATFORM_TOOLSET "LLVM-vs2014" CACHE STRING "Platform Toolset" FORCE)
 if (CMAKE_VS_PLATFORM_TOOLSET STREQUAL LLVM-vs2014)
  add_compile_options (-Wno-c++11-narrowing)
 endif ()
endif ()

#*TODO*: use pkg-config instead
if (WIN32)
 target_link_libraries (${PROJECT_NAME}Image ${ACE_LIBRARY})

# libpng
 target_link_libraries (${PROJECT_NAME}Image ${CMAKE_CURRENT_SOURCE_DIR}/../../../libpng-1.6.25/projects/vstudio/Debug/libpng16.lib)
# ffmpeg
 target_link_libraries (${PROJECT_NAME}Image ${CMAKE_CURRENT_SOURCE_DIR}/../../../ffmpeg/libavcodec/avcodec.lib)
 target_link_libraries (${PROJECT_NAME}Image ${CMAKE_CURRENT_SOURCE_DIR}/../../../ffmpeg/libavutil/avutil.lib)

# target_link_libraries (${PROJECT_NAME}Image ${COMMON_TARGET_NAME})
endif ()

# *WARNING*: linking libglade-2.dll(.a) requires "/SAFESEH:NO""
if (WIN32)
# set_target_properties (${PROJECT_NAME}Image PROPERTIES LINK_FLAGS /SAFESEH:NO)
endif ()
#set_target_properties (${PROJECT_NAME}Image PROPERTIES DEFINE_SYMBOL COMMON_IMAGE_BUILD_DLL)
set_target_properties (${PROJECT_NAME}Image PROPERTIES
                       VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
                       SOVERSION 1)

# add PCH
set_target_properties (${PROJECT_NAME}Image PROPERTIES
                       COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}Image)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}Image
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         common_image_exports.h
         common_image_tools.h
         stdafx.h
         DESTINATION ${INSTALL_INCLUDE_DIR})
install (TARGETS ${PROJECT_NAME}Image
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
         LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
#         RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
         COMPONENT dev)
