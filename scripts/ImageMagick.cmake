set (IMAGEMAGICK_SUPPORT_DEFAULT ON)
set (IMAGEMAGICK_IS_GRAPHICSMAGIC FALSE)
if (UNIX)
# find_package (ImageMagick COMPONENTS MagickCore MagickWand)
 find_package (ImageMagick)
 if (ImageMagick_FOUND)
  message (STATUS "found ImageMagick")
  set (IMAGEMAGICK_FOUND TRUE)
  set (ImageMagick_INCLUDE_DIRS "/usr/include/ImageMagick-6")
  set (ImageMagick_LIBRARIES "MagickCore-6.Q16;MagickWand-6.Q16")
 endif (ImageMagick_FOUND)
elseif (WIN32)
 if (VCPKG_USE)
#  cmake_policy (SET CMP0074 OLD)
  find_package (unofficial-graphicsmagick CONFIG)
  if (unofficial-graphicsmagick_FOUND)
   message (STATUS "found graphicsmagick")
   set (IMAGEMAGICK_FOUND TRUE)
   set (IMAGEMAGICK_IS_GRAPHICSMAGIC TRUE)
   get_target_property (ImageMagick_INCLUDE_DIRS unofficial::graphicsmagick::graphicsmagick INTERFACE_INCLUDE_DIRECTORIES)
   if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
       CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    get_target_property (ImageMagick_LIBRARIES unofficial::graphicsmagick::graphicsmagick IMPORTED_IMPLIB_DEBUG)
    get_target_property (ImageMagick_MagickCore_LIBRARY unofficial::graphicsmagick::graphicsmagick IMPORTED_IMPLIB_DEBUG)
    set (ImageMagick_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/debug/bin")
   else ()
    get_target_property (ImageMagick_LIBRARIES unofficial::graphicsmagick::graphicsmagick IMPORTED_IMPLIB_RELEASE)
    get_target_property (ImageMagick_MagickCore_LIBRARY unofficial::graphicsmagick::graphicsmagick IMPORTED_IMPLIB_RELEASE)
    set (ImageMagick_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin")
   endif ()
  endif (unofficial-graphicsmagick_FOUND)
 endif (VCPKG_USE)
 if (NOT unofficial-graphicsmagick_FOUND)
  find_library (ImageMagick_MagickCore_LIBRARY CORE_RL_MagickCore_.lib
                PATHS $ENV{LIB_ROOT}/ImageMagick
                PATH_SUFFIXES lib
                DOC "searching for CORE_RL_MagickCore_.lib"
                NO_DEFAULT_PATH)
#  find_library (ImageMagick_LIBRARY CORE_RL_Magick++_.lib
#                PATHS $ENV{LIB_ROOT}/ImageMagick
#                PATH_SUFFIXES lib
#                DOC "searching for CORE_RL_Magick++_.lib"
#                NO_DEFAULT_PATH)
  find_library (ImageMagick_MagickWand_LIBRARY CORE_RL_MagickWand_.lib
                PATHS $ENV{LIB_ROOT}/ImageMagick
                PATH_SUFFIXES lib
                DOC "searching for CORE_RL_MagickWand_.lib"
                NO_DEFAULT_PATH)
  if (NOT ImageMagick_MagickCore_LIBRARY)
   message (WARNING "could not find CORE_RL_MagickCore_.lib, continuing")
  endif (NOT ImageMagick_MagickCore_LIBRARY)
  if (NOT ImageMagick_MagickWand_LIBRARY)
   message (WARNING "could not find CORE_RL_MagickWand_.lib, continuing")
  endif (NOT ImageMagick_MagickWand_LIBRARY)
  if (ImageMagick_MagickCore_LIBRARY AND ImageMagick_MagickWand_LIBRARY)
   message (STATUS "found ImageMagick")
   set (IMAGEMAGICK_FOUND TRUE)
   set (ImageMagick_INCLUDE_DIRS "$ENV{LIB_ROOT}/ImageMagick/include")
   set (ImageMagick_LIBRARIES "${ImageMagick_MagickCore_LIBRARY};${ImageMagick_MagickWand_LIBRARY}")
   set (ImageMagick_LIB_DIR "$ENV{LIB_ROOT}/ImageMagick")
  endif (ImageMagick_MagickCore_LIBRARY AND ImageMagick_MagickWand_LIBRARY)
 endif (NOT unofficial-graphicsmagick_FOUND)
endif ()
if (IMAGEMAGICK_FOUND)
 option (IMAGEMAGICK_SUPPORT "enable ImageMagick support" ${IMAGEMAGICK_SUPPORT_DEFAULT})
 if (IMAGEMAGICK_SUPPORT)
  add_definitions (-DIMAGEMAGICK_SUPPORT)
# *NOTE*: get rid of compiler warnings
  if (NOT IMAGEMAGICK_IS_GRAPHICSMAGIC)
# Find Imagemagick Library directory
   get_filename_component (MAGICK_LIB_DIR ${ImageMagick_MagickCore_LIBRARY} DIRECTORY)
# Find where Magick-config lives
#   file (GLOB_RECURSE MAGICK_CONFIG FOLLOW_SYMLINKS ${MAGICK_LIB_DIR}/Magick++-config)
   file (GLOB_RECURSE MAGICK_CONFIG FOLLOW_SYMLINKS /usr/bin/Magick-config)
# Ask about CXX and lib flags/locations
   set (MAGICK_CONFIG ${MAGICK_CONFIG} CACHE STRING "Path to Magick-config utility")
   execute_process (COMMAND "${MAGICK_CONFIG}" "--cppflags" OUTPUT_VARIABLE MAGICK_CXX_FLAGS)
   execute_process (COMMAND "${MAGICK_CONFIG}" "--libs" OUTPUT_VARIABLE MAGICK_LD_FLAGS)
# Add these to cache
   set (MAGICK_CXX_FLAGS "${MAGICK_CXX_FLAGS}" CACHE STRING "ImageMagick configuration specific compilation flags." )
   set (MAGICK_LD_FLAGS  "${MAGICK_LD_FLAGS}" CACHE STRING "ImageMagick configuration specific linking flags.")
# Split into list
   string (REGEX MATCHALL "([^\ ]+)" MAGICK_CXX_FLAGS "${MAGICK_CXX_FLAGS}")
   string (REGEX MATCHALL "([^\ ]+)" MAGICK_LD_FLAGS "${MAGICK_LD_FLAGS}")
# Remove trailing whitespace (CMAKE warns about this)
   string (STRIP "${MAGICK_CXX_FLAGS}" MAGICK_CXX_FLAGS)
   string (STRIP "${MAGICK_LD_FLAGS}" MAGICK_LD_FLAGS)
  endif (NOT IMAGEMAGICK_IS_GRAPHICSMAGIC)
#  include_directories (${ImageMagick_INCLUDE_DIRS})
 endif (IMAGEMAGICK_SUPPORT)
endif (IMAGEMAGICK_FOUND)
