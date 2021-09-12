if (UNIX)
 find_package (ImageMagick COMPONENTS MagickCore MagickWand)
 if (ImageMagick_FOUND)
  set (IMAGEMAGICK_SUPPORT TRUE)
  add_definitions (-DIMAGEMAGICK_SUPPORT)
 endif (ImageMagick_FOUND)
elseif (WIN32)
 if (VCPKG_SUPPORT)
  find_package (unofficial-graphicsmagick CONFIG)
  if (graphicsmagick_FOUND)
   set (IMAGEMAGICK_SUPPORT TRUE)
   add_definitions (-DIMAGEMAGICK_SUPPORT)
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
  set (IMAGEMAGICK_SUPPORT TRUE)
  add_definitions (-DIMAGEMAGICK_SUPPORT)
 endif (VCPKG_SUPPORT)
endif ()
