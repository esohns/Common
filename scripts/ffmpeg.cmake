set (FFMPEG_SUPPORT_DEFAULT ON)
if (UNIX)
 pkg_check_modules (PKG_FFMPEG libavcodec libavformat libswscale libavutil)
 if (PKG_FFMPEG_FOUND)
  set (FFMPEG_FOUND TRUE)
  set (ffmpeg_INCLUDE_DIRS "${PKG_FFMPEG_INCLUDE_DIRS}")
  set (ffmpeg_LIBRARIES "${PKG_FFMPEG_LIBRARIES}")
 endif (PKG_FFMPEG_FOUND)
elseif (WIN32)
 if (VCPKG_SUPPORT)
#  cmake_policy (SET CMP0074 OLD)
  find_package (ffmpeg CONFIG)
  if (ffmpeg_FOUND)
   set (FFMPEG_FOUND TRUE)
   if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
       CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set (ffmpeg_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/debug/bin")
   else ()
    set (ffmpeg_LIB_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/bin")
   endif ()
  endif (ffmpeg_FOUND)
 endif (VCPKG_SUPPORT)
 if (NOT FFMPEG_FOUND)
  find_library (FFMPEG_LIBRARY_AVCODEC avcodec.lib
                PATHS $ENV{LIB_ROOT}/ffmpeg
                PATH_SUFFIXES lib
                DOC "searching for avcodec.lib"
                NO_DEFAULT_PATH)
  find_library (FFMPEG_LIBRARY_AVFORMAT avformat.lib
                PATHS $ENV{LIB_ROOT}/ffmpeg
                PATH_SUFFIXES lib
                DOC "searching for avformat.lib"
                NO_DEFAULT_PATH)
  find_library (FFMPEG_LIBRARY_SWSCALE swscale.lib
                PATHS $ENV{LIB_ROOT}/ffmpeg
                PATH_SUFFIXES lib
                DOC "searching for swscale.lib"
                NO_DEFAULT_PATH)
  find_library (FFMPEG_LIBRARY_AVUTIL avutil.lib
                PATHS $ENV{LIB_ROOT}/ffmpeg
                PATH_SUFFIXES lib
                DOC "searching for avutil.lib"
                NO_DEFAULT_PATH)
  if (NOT FFMPEG_LIBRARY_AVCODEC)
   message (WARNING "could not find avcodec.lib, continuing")
  else ()
   message (STATUS "Found avcodec.lib library \"${FFMPEG_LIBRARY_AVCODEC}\"")
  endif (NOT FFMPEG_LIBRARY_AVCODEC)
  if (NOT FFMPEG_LIBRARY_AVFORMAT)
   message (WARNING "could not find avformat.lib, continuing")
  else ()
   message (STATUS "Found avformat.lib library \"${FFMPEG_LIBRARY_AVFORMAT}\"")
  endif (NOT FFMPEG_LIBRARY_AVFORMAT)
  if (NOT FFMPEG_LIBRARY_SWSCALE)
   message (WARNING "could not find swscale.lib, continuing")
  else ()
   message (STATUS "Found swscale.lib library \"${FFMPEG_LIBRARY_SWSCALE}\"")
  endif (NOT FFMPEG_LIBRARY_SWSCALE)
  if (NOT FFMPEG_LIBRARY_AVUTIL)
   message (WARNING "could not find avutil.lib, continuing")
  else ()
   message (STATUS "Found avutil.lib library \"${FFMPEG_LIBRARY_AVUTIL}\"")
  endif (NOT FFMPEG_LIBRARY_AVUTIL)
  if (FFMPEG_LIBRARY_AVCODEC AND FFMPEG_LIBRARY_AVFORMAT AND FFMPEG_LIBRARY_SWSCALE AND FFMPEG_LIBRARY_AVUTIL)
   set (FFMPEG_FOUND TRUE)
   set (ffmpeg_INCLUDE_DIRS "$ENV{LIB_ROOT}/ffmpeg/include")
   set (ffmpeg_LIBRARIES "${FFMPEG_LIBRARY_AVCODEC};${FFMPEG_LIBRARY_AVFORMAT};${FFMPEG_LIBRARY_SWSCALE};${FFMPEG_LIBRARY_AVUTIL}")
   set (ffmpeg_LIB_DIR "$ENV{LIB_ROOT}/ffmpeg/bin")
  endif (FFMPEG_LIBRARY_AVCODEC AND FFMPEG_LIBRARY_AVFORMAT AND FFMPEG_LIBRARY_SWSCALE AND FFMPEG_LIBRARY_AVUTIL)
 endif (NOT FFMPEG_FOUND)
endif ()
if (FFMPEG_FOUND)
 option (FFMPEG_SUPPORT "enable ffmpeg support" ${FFMPEG_SUPPORT_DEFAULT})
 if (FFMPEG_SUPPORT)
  add_definitions (-DFFMPEG_SUPPORT)
#  include_directories (${ffmpeg_INCLUDE_DIRS})
  if (WIN32)
   add_link_options ("$<$<CXX_COMPILER_ID:MSVC>:-SAFESEH:NO>")
  endif (WIN32)
 endif (FFMPEG_SUPPORT)
endif (FFMPEG_FOUND)
