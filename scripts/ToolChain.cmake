#include(CMakeForceCompiler) #Disables checks

# this one is important
if (UNIX)
 set (CMAKE_SYSTEM_NAME Linux)
elseif (WIN32)
 set (CMAKE_SYSTEM_NAME Windows)
endif ()
#this one not so much
set (CMAKE_SYSTEM_VERSION 1)

#option (LLVM_ENABLE_DEBUG "Compile a debug version when using LLVM/clang" NO)

#set (CMAKE_SYSROOT /)
#set (CMAKE_STAGING_PREFIX /)

# specify the cross compiler
if (UNIX)
 if (LLVM_USE)
#CMAKE_FORCE_C_COMPILER( /usr/bin/clang CLang )
#CMAKE_FORCE_CXX_COMPILER ( /usr/bin/clang++ CLang )
  set (CMAKE_C_COMPILER   /usr/bin/clang)
  set (CMAKE_CXX_COMPILER /usr/bin/clang++)

  set (CMAKE_RANLIB "/usr/bin/llvm-ranlib" CACHE INTERNAL STRING)
  set (CMAKE_AR "/usr/bin/llvm-ar" CACHE INTERNAL STRING)
  set (CMAKE_LD "/usr/bin/llvm-link" CACHE INTERNAL STRING)
  set (CMAKE_LINKER "/usr/bin/llvm-link" CACHE INTERNAL STRING)
  set (CMAKE_C_LINKER "/usr/bin/llvm-ld")
  set (CMAKE_CXX_LINKER "/usr/bin/llvm-ld")
  set (CMAKE_TOOLCHAIN_PREFIX "llvm-")

# *NOTE*: LLVM defaults to 'gnu99' mode, gcc defaults to 'gnu89'
#set (CMAKE_C_FLAGS "-std=gnu89 -g -emit-llvm -D_GNU_SOURCE ${CMAKE_C_FLAGS} ${LLVM_C_COMPILE_FLAGS}")
#set (CMAKE_CXX_FLAGS "-std=gnu89 -g -emit-llvm -D_GNU_SOURCE ${CMAKE_CXX_FLAGS} ${LLVM_CXX_COMPILE_FLAGS}")
#set (CMAKE_C_FLAGS "-g -emit-llvm -D_GNU_SOURCE ${CMAKE_C_FLAGS} ${LLVM_C_COMPILE_FLAGS}")
#set (CMAKE_CXX_FLAGS "-g -emit-llvm -D_GNU_SOURCE ${CMAKE_CXX_FLAGS} ${LLVM_CXX_COMPILE_FLAGS}")
#set (CMAKE_C_LINK_EXECUTABLE "/usr/bin/llvm-link <OBJECTS> -o  <TARGET> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES>")
#set (CMAKE_CXX_LINK_EXECUTABLE "/usr/bin/llvm-link <OBJECTS> -o  <TARGET> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES>")

  set (CMAKE_C_FLAGS_INIT                "-Wall -std=c99")
  set (CMAKE_C_FLAGS_DEBUG_INIT          "-g")
  set (CMAKE_C_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
  set (CMAKE_C_FLAGS_RELEASE_INIT        "-O4 -DNDEBUG")
  set (CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

  set (CMAKE_CXX_FLAGS_INIT                "-Wall")
  set (CMAKE_CXX_FLAGS_DEBUG_INIT          "-g")
  set (CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
  set (CMAKE_CXX_FLAGS_RELEASE_INIT        "-O4 -DNDEBUG")
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

  message (STATUS "LLVM C Compiler: " ${CMAKE_C_COMPILER})
  message (STATUS "LLVM CXX Compiler: " ${CMAKE_CXX_COMPILER})
  message (STATUS "LLVM CFLAGS: " ${LLVM_C_COMPILE_FLAGS})
  message (STATUS "LLVM CXX flags: " ${LLVM_CXX_COMPILE_FLAGS})
  message (STATUS "LLVM flags: " ${LLVM_COMPILE_FLAGS})
  message (STATUS "LLVM LD flags: " ${LLVM_LDFLAGS})
  message (STATUS "LLVM core libs: " ${LLVM_LIBS_CORE})
  message (STATUS "LLVM JIT libs: " ${LLVM_LIBS_JIT})
  message (STATUS "LLVM JIT objs: " ${LLVM_LIBS_JIT_OBJECTS})

# where is the target environment 
#set (CMAKE_FIND_ROOT_PATH)

# search for programs in the build host directories
  set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)

# for libraries and headers in the target directories
  set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
  set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
  set (CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
 endif (LLVM_USE)
elseif (WIN32)
endif ()

