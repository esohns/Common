if (UNIX)
 set (GLEW_LIB_FILE libglew.so)
 find_library (GLEW_LIBRARY ${GLEW_LIB_FILE}
               PATHS /usr /usr/lib
               PATH_SUFFIXES lib64 x86_64-linux-gnu
               DOC "searching for ${GLEW_LIB_FILE}")
 if (GLEW_LIBRARY)
  message (STATUS "Found GLEW library \"${GLEW_LIBRARY}\"")
  set (GLEW_FOUND TRUE)
#   set (GLEW_INCLUDE_DIR "/usr/include")
  set (GLEW_LIBRARIES "${GLEW_LIBRARY}")
 else ()
  message (WARNING "could not find ${GLEW_LIB_FILE}, continuing")
 endif (GLEW_LIBRARY)
elseif (WIN32)
 if (VCPKG_USE)
  find_package (GLEW)
  if (GLEW_FOUND)
   message (STATUS "found GLEW")
   set (GLEW_INCLUDE_DIRS ${VCPKG_INCLUDE_DIR_BASE})
   set (GLEW_LIBRARIES "GLEW::GLEW")
   set (GLEW_LIB_DIR ${VCPKG_LIB_DIR}/bin)
  endif (GLEW_FOUND)
 endif (VCPKG_USE)
 if (NOT GLEW_FOUND)
  set (GLEW_LIB_FILE glew32.lib)
  set (BIN_PATH_BASE "glew/bin/Release")
  set (LIB_PATH_BASE "glew/lib/Release")
  set (LIB_PATH_SUFFIX "Win32")
  if (${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE} STREQUAL "x64")
   set (LIB_PATH_SUFFIX "x64")
  endif (${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE} STREQUAL "x64")
  find_library (GLEW_LIBRARY ${GLEW_LIB_FILE}
                PATHS $ENV{LIB_ROOT}/${LIB_PATH_BASE}
                PATH_SUFFIXES ${LIB_PATH_SUFFIX}
                DOC "searching for ${GLEW_LIB_FILE}")
  if (GLEW_LIBRARY)
   message (STATUS "found GLEW library \"${GLEW_LIBRARY}\"")
  set (GLEW_FOUND TRUE)
  set (GLEW_INCLUDE_DIRS "$ENV{LIB_ROOT}/glew/include")
  set (GLEW_LIBRARIES ${GLEW_LIBRARY})
  set (GLEW_LIB_DIR "$ENV{LIB_ROOT}/${BIN_PATH_BASE}/${LIB_PATH_SUFFIX}")
  else ()
   message (WARNING "could not find ${GLEW_LIB_FILE}, continuing")
  endif (GLEW_LIBRARY)
 endif (NOT GLEW_FOUND)
endif ()
if (GLEW_FOUND)
 option (GLEW_SUPPORT "enable OpenGL GLEW support" ON)
 if (GLEW_SUPPORT)
  add_definitions (-DGLEW_SUPPORT)
 endif (GLEW_SUPPORT)
endif (GLEW_FOUND)