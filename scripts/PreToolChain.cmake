if (UNIX)
 find_program (GCC_BIN
               gcc
               HINTS /usr/bin
               PATHS /usr
               PATH_SUFFIXES bin
               DOC "GNU gcc"
               #NO_DEFAULT_PATH
               #NO_CMAKE_ENVIRONMENT_PATH
               #NO_CMAKE_PATH
               #NO_SYSTEM_ENVIRONMENT_PATH
               #NO_CMAKE_SYSTEM_PATH
               #CMAKE_FIND_ROOT_PATH_BOTH |
               #ONLY_CMAKE_FIND_ROOT_PATH |
               #NO_CMAKE_FIND_ROOT_PATH
              )
 if (GCC_BIN)
  get_filename_component (GCC_PATH_ROOT ${GCC_BIN} PATH)
  set (GCC_SUPPORT ON)
 endif (GCC_BIN)
 
 find_program (LLVM_BIN
               clang++
#               HINTS /usr/bin
               PATHS /usr
               PATH_SUFFIXES bin
               DOC "LLVM clang++"
               #NO_DEFAULT_PATH
               #NO_CMAKE_ENVIRONMENT_PATH
               #NO_CMAKE_PATH
               #NO_SYSTEM_ENVIRONMENT_PATH
               #NO_CMAKE_SYSTEM_PATH
               #CMAKE_FIND_ROOT_PATH_BOTH |
               #ONLY_CMAKE_FIND_ROOT_PATH |
               #NO_CMAKE_FIND_ROOT_PATH
              )
 if (LLVM_BIN)
  get_filename_component (LLVM_PATH_ROOT ${LLVM_BIN} PATH)
  set (LLVM_SUPPORT ON)
 endif (LLVM_BIN)
elseif (WIN32)
 set (LLVM_PATH_ROOT "C:/Program Files/LLVM")
 find_program (LLVM_BIN
               "clang++.exe"
               #HINTS ${LLVM_PATH_ROOT}/bin
               PATHS ${LLVM_PATH_ROOT}
               PATH_SUFFIXES bin
               DOC "LLVM clang++.exe"
               #NO_DEFAULT_PATH
               #NO_CMAKE_ENVIRONMENT_PATH
               #NO_CMAKE_PATH
               #NO_SYSTEM_ENVIRONMENT_PATH
               #NO_CMAKE_SYSTEM_PATH
               #CMAKE_FIND_ROOT_PATH_BOTH |
               #ONLY_CMAKE_FIND_ROOT_PATH |
               #NO_CMAKE_FIND_ROOT_PATH
              )
 if (LLVM_BIN)
  get_filename_component (LLVM_PATH_ROOT ${LLVM_BIN} PATH)
  set (LLVM_SUPPORT ON)
 endif (LLVM_BIN)

 if (${CMAKE_GENERATOR} MATCHES "^Visual Studio(.)*$")
  string (REGEX MATCHALL "^Visual Studio ([0-9]+) ([0-9]+)$" match ${CMAKE_GENERATOR})
  set (MSVC_RELEASE ${CMAKE_MATCH_1})
  set (MSVC_EDITION ${CMAKE_MATCH_2})

  message (STATUS "MSVC_VERSION: ${MSVC_VERSION}")
  message (STATUS "MSVC_RELEASE: ${MSVC_RELEASE}")
  message (STATUS "MSVC_EDITION: ${MSVC_EDITION}")

  if (MSVC_RELEASE EQUAL 18) # MSVC 2026
    set (MSVC_PATH_ROOT "C:/Program Files/Microsoft Visual Studio/${MSVC_RELEASE}")
  elseif (MSVC_RELEASE GREATER 10) # MSVC 10 (*TODO*: inaccurate)
   set (MSVC_PATH_ROOT "C:/Program Files/Microsoft Visual Studio/${MSVC_EDITION}")
#   message (STATUS "MSVC_BIN_PATH: \"${MSVC_BIN_PATH}\"")
#  elseif (MSVC_VERSION EQUAL 1600) # MSVC 10
  elseif (MSVC_RELEASE EQUAL 10) # MSVC 10
   set (MSVC_PATH_ROOT "C:/Program Files/Microsoft Visual Studio 10.0")
   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/VC/bin")
  # message (STATUS "MSVC_BIN_PATH: \"${MSVC_BIN_PATH}\"")
  else ()
   message (FATAL_ERROR "MSVC toolchain version (was: \"${MSVC_VERSION}\") not (yet) supported")
  endif ()
 endif (${CMAKE_GENERATOR} MATCHES "^Visual Studio(.)*$")

 find_program (CL_PATH
               "cl.exe"
#                HINTS ${MSVC_BIN_PATH}
               PATHS ${MSVC_PATH_ROOT}
               PATH_SUFFIXES ${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}
               DOC "MSVC cl.exe"
               #NO_DEFAULT_PATH
               #NO_CMAKE_ENVIRONMENT_PATH
               #NO_CMAKE_PATH
               #NO_SYSTEM_ENVIRONMENT_PATH
               #NO_CMAKE_SYSTEM_PATH
               #CMAKE_FIND_ROOT_PATH_BOTH |
               #ONLY_CMAKE_FIND_ROOT_PATH |
               #NO_CMAKE_FIND_ROOT_PATH
              )
 if (CL_PATH OR MSVC)
  set (MSVC_SUPPORT ON)
 endif (CL_PATH OR MSVC)
endif ()
