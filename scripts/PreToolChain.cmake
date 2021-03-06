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
  set (GCC_SUPPORT ON)
 endif (GCC_BIN)
 
 find_program (LLVM_BIN
               clang++
               HINTS /usr/bin
               PATHS /usr
               PATH_SUFFIXES bin
               DOC "LLVM clang"
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
  set (LLVM_SUPPORT ON)
 endif (LLVM_BIN)
elseif (WIN32)
# message (STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
# if (MSVC)
 if (${CMAKE_GENERATOR} MATCHES "^Visual Studio(.)*$")
  set (COMPILER_EXE "cl.exe" CACHE STRING "compiler")
  string (REGEX MATCHALL "^Visual Studio ([0-9]+) ([0-9]+)$" match ${CMAKE_GENERATOR})
#  foreach (_var IN LISTS match)
#   message ("${_var}")
#  endforeach ()
  set (MSVC_RELEASE ${CMAKE_MATCH_1})
  set (MSVC_EDITION ${CMAKE_MATCH_2})
  else ()
   message (FATAL_ERROR "invalid/unknown MSVC generator (was: \"${CMAKE_GENERATOR}\")")
  endif ()
#  message (STATUS "MSVC_VERSION: ${MSVC_VERSION}")
#  message (STATUS "MSVC_RELEASE: ${MSVC_RELEASE}")
#  message (STATUS "MSVC_EDITION: ${MSVC_EDITION}")

#  if (MSVC_VERSION GREATER 1600) # MSVC 10 (*TODO*: inaccurate)
  if (MSVC_RELEASE GREATER 10) # MSVC 10 (*TODO*: inaccurate)
   set (MSVC_PATH_ROOT "C:/Program Files (x86)/Microsoft Visual Studio")
#   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/2017/Community/VC/Tools/MSVC/14.14.26428/bin/Hostx64/x86")
#   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/2017/Community/VC/Tools/MSVC/14.16.27023/bin/Hostx64/x86")
#   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/2019/Community/VC/Tools/MSVC/14.20.27508/bin/Hostx64/x86")
#   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/2019/Community/VC/Tools/MSVC/14.21.27702/bin/Hostx64/x86")
#   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/2019/Community/VC/Tools/MSVC/14.22.27905/bin/Hostx64/x86")
#   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/2019/Community/VC/Tools/MSVC/14.23.28105/bin/Hostx64/x86")
#   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/2019/Community/VC/Tools/MSVC/14.25.28610/bin/Hostx64/x86")
#   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/2019/Community/VC/Tools/MSVC/14.26.28801/bin/Hostx64/x86")
#   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/2019/Community/VC/Tools/MSVC/14.28.29333/bin/Hostx64/x86")
   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/2019/Community/VC/Tools/MSVC/14.28.29910/bin/Hostx64/x86")
#  elseif (MSVC_VERSION EQUAL 1600) # MSVC 10
  elseif (MSVC_RELEASE EQUAL 10) # MSVC 10
#   message (STATUS "MSVC_VERSION: ${MSVC_VERSION}")
   set (MSVC_PATH_ROOT "C:/Program Files/Microsoft Visual Studio 10.0")
   set (MSVC_BIN_PATH "${MSVC_PATH_ROOT}/VC/bin")
  else ()
   message (FATAL_ERROR "MSVC toolchain version (was: \"${MSVC_VERSION}\") not (yet) supported")
  endif ()
  find_program (COMPILER_PATH
                ${COMPILER_EXE}
                HINTS ${MSVC_BIN_PATH}
                PATHS ${MSVC_BIN_PATH}
#                PATH_SUFFIXES x64 x86
                DOC "MSVC ${COMPILER_EXE}"
                #NO_DEFAULT_PATH
                #NO_CMAKE_ENVIRONMENT_PATH
                #NO_CMAKE_PATH
                #NO_SYSTEM_ENVIRONMENT_PATH
                #NO_CMAKE_SYSTEM_PATH
                #CMAKE_FIND_ROOT_PATH_BOTH |
                #ONLY_CMAKE_FIND_ROOT_PATH |
                #NO_CMAKE_FIND_ROOT_PATH
               )
  if (COMPILER_PATH)
#   message (STATUS "found ${COMPILER_EXE}: ${COMPILER_PATH}")
   set (MSVC_SUPPORT ON)
  endif (COMPILER_PATH)
 #elseif ()
 # message (FATAL_ERROR "toolchain not (yet) supported")
 endif ()
endif ()

