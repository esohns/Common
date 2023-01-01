set (FFMPEG_SUPPORT_DEFAULT ON)
if (UNIX)
# set (ENV{PKG_CONFIG_PATH} "/usr/local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
 pkg_check_modules (PKG_FFMPEG libavcodec libavformat libswscale libavutil)
 pkg_check_modules (PKG_FFMPEG_2 libswresample)
 if (PKG_FFMPEG_FOUND)
  set (FFMPEG_FOUND TRUE)
  set (ffmpeg_INCLUDE_DIRS "${PKG_FFMPEG_INCLUDE_DIRS}")
  set (ffmpeg_LIBRARIES "${PKG_FFMPEG_LIBRARIES}")
 endif (PKG_FFMPEG_FOUND)
 if (PKG_FFMPEG_2_FOUND)
  set (FFMPEG_2_FOUND TRUE)
  set (ffmpeg_2_INCLUDE_DIRS "${PKG_FFMPEG_2_INCLUDE_DIRS}")
  set (ffmpeg_2_LIBRARIES "${PKG_FFMPEG_2_LIBRARIES}")
 endif (PKG_FFMPEG_2_FOUND)
elseif (WIN32)
 if (VCPKG_SUPPORT)
#  cmake_policy (SET CMP0074 OLD)
  find_package (ffmpeg
                COMPONENTS core)
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
  if (EXISTS $ENV{FFMPEG_ROOT})
   set (FFMPEG_ROOT $ENV{FFMPEG_ROOT})
  else ()
   set (FFMPEG_ROOT $ENV{LIB_ROOT}/ffmpeg)
  endif (EXISTS $ENV{FFMPEG_ROOT})
  find_library (FFMPEG_LIBRARY_AVCODEC avcodec.lib
                PATHS ${FFMPEG_ROOT}
                PATH_SUFFIXES libavcodec lib
                DOC "searching for avcodec.lib"
                NO_DEFAULT_PATH)
  find_library (FFMPEG_LIBRARY_AVFORMAT avformat.lib
                PATHS ${FFMPEG_ROOT}
                PATH_SUFFIXES libavformat lib
                DOC "searching for avformat.lib"
                NO_DEFAULT_PATH)
  find_library (FFMPEG_LIBRARY_SWSCALE swscale.lib
                PATHS ${FFMPEG_ROOT}
                PATH_SUFFIXES libswscale lib
                DOC "searching for swscale.lib"
                NO_DEFAULT_PATH)
  find_library (FFMPEG_LIBRARY_AVUTIL avutil.lib
                PATHS ${FFMPEG_ROOT}
                PATH_SUFFIXES libavutil lib
                DOC "searching for avutil.lib"
                NO_DEFAULT_PATH)
  find_library (FFMPEG_LIBRARY_SWRESAMPLE swresample.lib
                PATHS ${FFMPEG_ROOT}
                PATH_SUFFIXES libswresample lib
                DOC "searching for swresample.lib"
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
  if (NOT FFMPEG_LIBRARY_SWRESAMPLE)
   message (WARNING "could not find swresample.lib, continuing")
  else ()
   message (STATUS "Found swresample.lib library \"${FFMPEG_LIBRARY_SWRESAMPLE}\"")
  endif (NOT FFMPEG_LIBRARY_SWRESAMPLE)
  if (FFMPEG_LIBRARY_AVCODEC AND FFMPEG_LIBRARY_AVFORMAT AND FFMPEG_LIBRARY_SWSCALE AND FFMPEG_LIBRARY_AVUTIL)
   set (FFMPEG_FOUND TRUE)
   set (ffmpeg_LIBRARIES "${FFMPEG_LIBRARY_AVCODEC};${FFMPEG_LIBRARY_AVFORMAT};${FFMPEG_LIBRARY_SWSCALE};${FFMPEG_LIBRARY_AVUTIL}")
#    set (ffmpeg_INCLUDE_DIRS "${FFMPEG_ROOT}")
#    set (ffmpeg_LIB_DIR "${FFMPEG_ROOT}/libavcodec;${FFMPEG_ROOT}/libavformat;${FFMPEG_ROOT}/libswscale;${FFMPEG_ROOT}/libswresample;${FFMPEG_ROOT}/libavutil" CACHE INTERNAL "")
   set (ffmpeg_INCLUDE_DIRS "${FFMPEG_ROOT}/include")
   set (ffmpeg_LIB_DIR "${FFMPEG_ROOT}/bin")
  endif (FFMPEG_LIBRARY_AVCODEC AND FFMPEG_LIBRARY_AVFORMAT AND FFMPEG_LIBRARY_SWSCALE AND FFMPEG_LIBRARY_AVUTIL)
  if (FFMPEG_LIBRARY_SWRESAMPLE)
   set (FFMPEG_2_FOUND TRUE)
   set (ffmpeg_2_LIBRARIES "${FFMPEG_LIBRARY_SWRESAMPLE}")
#    set (ffmpeg_2_INCLUDE_DIRS "${FFMPEG_ROOT}")
#    set (ffmpeg_2_LIB_DIR "${FFMPEG_ROOT}/libswresample" CACHE INTERNAL "")
   set (ffmpeg_2_INCLUDE_DIRS "${FFMPEG_ROOT}/include")
   set (ffmpeg_2_LIB_DIR "${FFMPEG_ROOT}/bin" CACHE INTERNAL "")
  endif (FFMPEG_LIBRARY_SWRESAMPLE)
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
