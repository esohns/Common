# specify the (cross-)compiler
if (UNIX)
 if (LLVM_USE)
#CMAKE_FORCE_C_COMPILER( /usr/bin/clang CLang )
#CMAKE_FORCE_CXX_COMPILER ( /usr/bin/clang++ CLang )
  set (CMAKE_C_COMPILER   "${LLVM_PATH_ROOT}/clang")
  set (CMAKE_CXX_COMPILER "${LLVM_BIN}")

  set (CMAKE_RANLIB "${LLVM_PATH_ROOT}/llvm-ranlib" CACHE INTERNAL STRING)
  set (CMAKE_AR "${LLVM_PATH_ROOT}/llvm-ar" CACHE INTERNAL STRING)
  set (CMAKE_LD "${LLVM_PATH_ROOT}/llvm-link" CACHE INTERNAL STRING)
  set (CMAKE_LINKER "${LLVM_PATH_ROOT}/llvm-link" CACHE INTERNAL STRING)
  set (CMAKE_C_LINKER "${LLVM_PATH_ROOT}/llvm-ld")
  set (CMAKE_CXX_LINKER "${LLVM_PATH_ROOT}/llvm-ld")
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

  execute_process (COMMAND ${LLVM_BIN} --version]
                   OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)
 endif (LLVM_USE)
elseif (WIN32)
 if (LLVM_USE)
#CMAKE_FORCE_C_COMPILER( C:/Program Files/LLVM/clang.exe CLang )
#CMAKE_FORCE_CXX_COMPILER ( C:/Program Files/LLVM/clang++.exe CLang )
  set (CMAKE_C_COMPILER   "clang.exe")
  set (CMAKE_CXX_COMPILER "clang++.exe")

  set (CMAKE_RANLIB "llvm-ranlib.exe" CACHE INTERNAL STRING)
  set (CMAKE_AR "llvm-ar.exe" CACHE INTERNAL STRING)
  set (CMAKE_LD "lld-link.exe" CACHE INTERNAL STRING)
  set (CMAKE_LINKER "lld-link.exe" CACHE INTERNAL STRING)
  set (CMAKE_C_LINKER "lld-link.exe")
  set (CMAKE_CXX_LINKER "lld-link.exe")
  set (CMAKE_TOOLCHAIN_PREFIX "llvm-")

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
 
  execute_process (COMMAND ${LLVM_BIN} --version]
                   OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)
 endif (LLVM_USE)
endif ()

message (STATUS "CMAKE_CXX_COMPILER_VERSION: " ${CMAKE_CXX_COMPILER_VERSION})
message (STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})
