set (IMAGEMAGICK_SUPPORT_DEFAULT ON)
if (UNIX)
 find_package (ImageMagick COMPONENTS MagickCore MagickWand)
 if (ImageMagick_FOUND)
  set (IMAGEMAGICK_FOUND TRUE)
 endif (ImageMagick_FOUND)
elseif (WIN32)
 if (VCPKG_SUPPORT)
#  cmake_policy (SET CMP0074 OLD)
  find_package (unofficial-graphicsmagick CONFIG)
  if (unofficial-graphicsmagick_FOUND)
   set (IMAGEMAGICK_FOUND TRUE)
   get_target_property (ImageMagick_INCLUDE_DIRS unofficial::graphicsmagick::graphicsmagick INTERFACE_INCLUDE_DIRECTORIES)
   set (ImageMagick_LIBRARIES $<TARGET_LINKER_FILE:unofficial::graphicsmagick::graphicsmagick>)
   if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
       CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set (ImageMagick_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/debug/bin")
   else ()
    set (ImageMagick_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin")
   endif ()
  endif (unofficial-graphicsmagick_FOUND)
 else ()
  find_library (IMAGEMAGICK_LIBRARY_CORE CORE_RL_MagickCore_.lib
                PATHS $ENV{LIB_ROOT}/ImageMagick
                PATH_SUFFIXES lib
                DOC "searching for CORE_RL_MagickCore_.lib"
                NO_DEFAULT_PATH)
  find_library (IMAGEMAGICK_LIBRARY CORE_RL_Magick++_.lib
                PATHS $ENV{LIB_ROOT}/ImageMagick
                PATH_SUFFIXES lib
                DOC "searching for CORE_RL_Magick++_.lib"
                NO_DEFAULT_PATH)
  find_library (IMAGEMAGICK_LIBRARY_WAND CORE_RL_MagickWand_.lib
                PATHS $ENV{LIB_ROOT}/ImageMagick
                PATH_SUFFIXES lib
                DOC "searching for CORE_RL_MagickWand_.lib"
                NO_DEFAULT_PATH)
  #if (NOT EXISTS IMAGEMAGICK_LIBRARY_CORE)
  if (NOT IMAGEMAGICK_LIBRARY_CORE)
   message (WARNING "could not find CORE_RL_MagickCore_.lib, continuing")
  else ()
   message (STATUS "Found CORE_RL_MagickCore_.lib library \"${IMAGEMAGICK_LIBRARY_CORE}\"")
  endif (NOT IMAGEMAGICK_LIBRARY_CORE)
  if (NOT IMAGEMAGICK_LIBRARY_WAND)
   message (WARNING "could not find CORE_RL_MagickWand_.lib, continuing")
  else ()
   message (STATUS "Found CORE_RL_MagickWand_.lib library \"${IMAGEMAGICK_LIBRARY_WAND}\"")
  endif (NOT IMAGEMAGICK_LIBRARY_WAND)
  if (IMAGEMAGICK_LIBRARY_CORE AND IMAGEMAGICK_LIBRARY_WAND)
   set (IMAGEMAGICK_FOUND TRUE)
   set (ImageMagick_INCLUDE_DIRS "$ENV{LIB_ROOT}/ImageMagick/include")
   set (ImageMagick_LIBRARIES "${IMAGEMAGICK_LIBRARY_CORE};${IMAGEMAGICK_LIBRARY_WAND}")
   set (ImageMagick_LIB_DIR "$ENV{LIB_ROOT}/ImageMagick")
  endif (IMAGEMAGICK_LIBRARY_CORE AND IMAGEMAGICK_LIBRARY_WAND)
 endif (VCPKG_SUPPORT)
endif ()
if (IMAGEMAGICK_FOUND)
 option (IMAGEMAGICK_SUPPORT "enable ImageMagick support" ${IMAGEMAGICK_SUPPORT_DEFAULT})
 if (IMAGEMAGICK_SUPPORT)
  add_definitions (-DIMAGEMAGICK_SUPPORT)
# *NOTE*: get rid of compiler warnings
# Find Imagemagick Library directory
  get_filename_component (MAGICK_LIB_DIR ${ImageMagick_MagickCore_LIBRARY} DIRECTORY)
# Find where Magick++-config lives
  file (GLOB_RECURSE MAGICK_CONFIG FOLLOW_SYMLINKS ${MAGICK_LIB_DIR}/Magick++-config)
# Ask about CXX and lib flags/locations
  set (MAGICK_CONFIG ${MAGICK_CONFIG} CACHE STRING "Path to Magick++-config utility")
  execute_process (COMMAND "${MAGICK_CONFIG}" "--cxxflags" OUTPUT_VARIABLE MAGICK_CXX_FLAGS)
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
#  include_directories (${ImageMagick_INCLUDE_DIRS})
 endif (IMAGEMAGICK_SUPPORT)
endif (IMAGEMAGICK_FOUND)
