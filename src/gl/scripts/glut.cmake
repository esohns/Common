# *TODO*: cmake-modules/FindGLUT.cmake is currently broken
set (CMAKE_MODULE_PATH_TMP ${CMAKE_MODULE_PATH})
unset (CMAKE_MODULE_PATH)
#cmake_policy (PUSH)
#cmake_policy (SET CMP0072 OLD)
#set (GLUT_FIND_QUIETLY TRUE)
find_package (GLUT MODULE)
if (NOT GLUT_FOUND)
 if (UNIX)
  set (GLUT_LIB_FILE libglut.so)
  find_library (GLUT_LIBRARY ${GLUT_LIB_FILE}
                PATHS /usr /usr/lib
                PATH_SUFFIXES lib64 x86_64-linux-gnu
                DOC "searching for ${GLUT_LIB_FILE}")
  if (GLUT_LIBRARY)
   message (STATUS "Found Glut library \"${GLUT_LIBRARY}\"")
   set (GLUT_FOUND TRUE)
#   set (GLUT_INCLUDE_DIR "/usr/include")
   set (GLUT_LIBRARIES "${GLUT_LIBRARY}")
  else ()
   message (WARNING "could not find ${GLUT_LIB_FILE}, continuing")
  endif (GLUT_LIBRARY)
 elseif (WIN32)
  if (VCPKG_USE)
   find_package (GLUT)
   if (GLUT_FOUND)
    message (STATUS "found GLUT")
    set (GLUT_INCLUDE_DIRS ${VCPKG_INCLUDE_DIR})
    set (GLUT_LIBRARIES "GLUT::GLUT")
    set (GLUT_LIB_DIR "${VCPKG_BIN_DIR}")
   endif (GLUT_FOUND)
  endif (VCPKG_USE)
  if (NOT GLUT_FOUND)
   set (GLUT_LIB_FILE freeglut.lib)
   set (BIN_PATH_SUFFIX "bin")
   set (LIB_PATH_BASE "freeglut")
   set (LIB_PATH_SUFFIX "lib")
   if (${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE} STREQUAL "x64")
    set (BIN_PATH_SUFFIX "${BIN_PATH_SUFFIX}/x64")
    set (LIB_PATH_BASE "${LIB_PATH_BASE}/lib")
    set (LIB_PATH_SUFFIX "x64")
   endif (${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE} STREQUAL "x64")
   find_library (GLUT_LIBRARY ${GLUT_LIB_FILE}
                 PATHS $ENV{LIB_ROOT}/${LIB_PATH_BASE}
                 PATH_SUFFIXES ${LIB_PATH_SUFFIX}
                 DOC "searching for ${GLUT_LIB_FILE}")
   if (GLUT_LIBRARY)
    message (STATUS "found Glut library \"${GLUT_LIBRARY}\"")
    set (GLUT_FOUND TRUE)
    set (GLUT_INCLUDE_DIRS "$ENV{LIB_ROOT}/freeglut/include")
    set (GLUT_LIBRARIES ${GLUT_LIBRARY})
    set (GLUT_LIB_DIR "$ENV{LIB_ROOT}/freeglut/${BIN_PATH_SUFFIX}")
   else ()
    message (WARNING "could not find ${GLUT_LIB_FILE}, continuing")
   endif (GLUT_LIBRARY)
  endif (NOT GLUT_FOUND)
 endif ()
endif (NOT GLUT_FOUND)
if (GLUT_FOUND)
 option (GLUT_SUPPORT "enable OpenGL GLUT support" ON)
 if (GLUT_SUPPORT)
  add_definitions (-DGLUT_SUPPORT)
 endif (GLUT_SUPPORT)
endif (GLUT_FOUND)
#cmake_policy (POP)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_TMP})
