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
   set (ImageMagick_INCLUDE_DIRS ${graphicsmagick_INCLUDE_DIRS})
   set (ImageMagick_LIBRARIES "unofficial::graphicsmagick::graphicsmagick")
   if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
       CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set (ImageMagick_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/debug/bin")
   else ()
    set (ImageMagick_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin")
   endif ()
  endif (graphicsmagick_FOUND)
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
  #if (NOT EXISTS ACEXML_LIBRARY)
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
 endif (IMAGEMAGICK_SUPPORT)
endif (IMAGEMAGICK_FOUND)
