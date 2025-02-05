if (UNIX)
 set (GLFW_LIB_FILE libglfw.so.3)
 find_library (GLFW_LIBRARY ${GLFW_LIB_FILE}
               PATHS /usr
               PATH_SUFFIXES lib64 x86_64-linux-gnu
               DOC "searching for ${GLFW_LIB_FILE}")
 if (GLFW_LIBRARY)
  message (STATUS "Found GLFW library \"${GLFW_LIBRARY}\"")
  set (GLFW_FOUND TRUE)
#  set (GLFW_INCLUDE_DIR "/usr/include")
  set (GLFW_LIBRARIES "${GLFW_LIBRARY}")
 else ()
  message (WARNING "could not find ${GLFW_LIB_FILE}, continuing")
 endif (GLFW_LIBRARY)
elseif (WIN32)
 if (VCPKG_USE)
  find_package (GLFW)
  if (GLFW_FOUND)
   message (STATUS "found GLFW")
   set (GLFW_INCLUDE_DIRS ${VCPKG_INCLUDE_DIR})
   set (GLFW_LIBRARIES "GLFW::GLFW")
   set (GLFW_LIB_DIR "${VCPKG_BIN_DIR}")
  endif (GLFW_FOUND)
 endif (VCPKG_USE)
 if (NOT GLFW_FOUND)
  set (GLFW_LIB_FILE glfw3dll.lib)
  set (LIB_PATH_BASE "lib-vc2022")
  find_library (GLFW_LIBRARY ${GLFW_LIB_FILE}
                PATHS $ENV{LIB_ROOT}/glfw
                PATH_SUFFIXES ${LIB_PATH_BASE}
                DOC "searching for ${GLFW_LIB_FILE}")
  if (GLFW_LIBRARY)
   message (STATUS "found GLFW library \"${GLFW_LIBRARY}\"")
  set (GLFW_FOUND TRUE)
  set (GLFW_INCLUDE_DIRS "$ENV{LIB_ROOT}/glfw/include")
  set (GLFW_LIBRARIES ${GLFW_LIBRARY})
  set (GLFW_LIB_DIR "$ENV{LIB_ROOT}/glfw/${LIB_PATH_BASE}")
  else ()
   message (WARNING "could not find ${GLFW_LIB_FILE}, continuing")
  endif (GLFW_LIBRARY)
 endif (NOT GLFW_FOUND)
endif ()
if (GLFW_FOUND)
 option (GLFW_SUPPORT "enable GLFW support" ON)
 if (GLFW_SUPPORT)
  add_definitions (-DGLFW_SUPPORT)
 endif (GLFW_SUPPORT)
endif (GLFW_FOUND)
