set (TENSORFLOW_SUPPORT_DEFAULT ON)
set (TENSORFLOW_CC_SUPPORT_DEFAULT OFF)
if (UNIX)
# set (ENV{PKG_CONFIG_PATH} "/usr/local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
 pkg_check_modules (PKG_TENSORFLOW tensorflow)
 if (PKG_TENSORFLOW_FOUND)
  set (TENSORFLOW_FOUND TRUE)
  set (tensorflow_INCLUDE_DIRS "${PKG_TENSORFLOW_INCLUDE_DIRS}")
  set (tensorflow_LIBRARIES "${PKG_TENSORFLOW_LIBRARIES}")
 else ()
  find_library (TENSORFLOW_LIBRARY libtensorflow.so
                PATHS /usr/local
                PATH_SUFFIXES lib
                DOC "searching for libtensorflow.so"
                NO_DEFAULT_PATH)
  if (NOT TENSORFLOW_LIBRARY)
   message (WARNING "could not find libtensorflow.so, continuing")
  else ()
   message (STATUS "Found libtensorflow.so library \"${TENSORFLOW_LIBRARY}\"")
  endif (NOT TENSORFLOW_LIBRARY)
  if (TENSORFLOW_LIBRARY)
   set (TENSORFLOW_FOUND TRUE)
   set (tensorflow_LIBRARIES "${TENSORFLOW_LIBRARY}")
   set (tensorflow_INCLUDE_DIRS "/usr/local/include")
   set (tensorflow_LIB_DIR "/usr/local/lib")
  endif (TENSORFLOW_LIBRARY)

  find_library (TENSORFLOW_CC_LIBRARY libtensorflow_cc.so
                PATHS /usr/local
                PATH_SUFFIXES lib
                DOC "searching for libtensorflow_cc.so"
                NO_DEFAULT_PATH)
  if (NOT TENSORFLOW_CC_LIBRARY)
   message (WARNING "could not find libtensorflow_cc.so, continuing")
  else ()
   message (STATUS "Found libtensorflow_cc.so library \"${TENSORFLOW_CC_LIBRARY}\"")
  endif (NOT TENSORFLOW_CC_LIBRARY)
  find_library (TENSORFLOW_FRAMEWORK_LIBRARY libtensorflow_framework.so.2
                PATHS /usr/local
                PATH_SUFFIXES lib
                DOC "searching for libtensorflow_framework.so.2"
                NO_DEFAULT_PATH)
  if (NOT TENSORFLOW_FRAMEWORK_LIBRARY)
   message (WARNING "could not find libtensorflow_framework.so.2, continuing")
  else ()
   message (STATUS "Found libtensorflow_framework.so.2 library \"${TENSORFLOW_FRAMEWORK_LIBRARY}\"")
  endif (NOT TENSORFLOW_FRAMEWORK_LIBRARY)
  if (TENSORFLOW_CC_LIBRARY AND TENSORFLOW_FRAMEWORK_LIBRARY)
   set (TENSORFLOW_CC_FOUND TRUE)
   set (tensorflow_cc_LIBRARIES "${TENSORFLOW_CC_LIBRARY};${TENSORFLOW_FRAMEWORK_LIBRARY}")
   set (tensorflow_cc_INCLUDE_DIRS "/usr/local/include")
   set (tensorflow_cc_LIB_DIR "/usr/local/lib")
  endif (TENSORFLOW_CC_LIBRARY AND TENSORFLOW_FRAMEWORK_LIBRARY)
 endif (PKG_TENSORFLOW_FOUND)
elseif (WIN32)
 if (VCPKG_USE)
#  cmake_policy (SET CMP0074 OLD)
  find_package (tensorflow CONFIG)
  if (tensorflow_FOUND)
   set (TENSORFLOW_FOUND TRUE)
   set (tensorflow_INCLUDE_DIRS ${VCPKG_INCLUDE_DIR})
   set (tensorflow_LIBRARIES ${VCPKG_LIB_DIR}/libtensorflow.lib)
   set (tensorflow_LIB_DIR "${VCPKG_BIN_DIR}")
  endif (tensorflow_FOUND)
 endif (VCPKG_USE)
 if (NOT TENSORFLOW_FOUND)
  if (EXISTS $ENV{TENSORFLOW_ROOT})
   set (TENSORFLOW_ROOT $ENV{TENSORFLOW_ROOT})
  else ()
   set (TENSORFLOW_ROOT $ENV{LIB_ROOT}/tensorflow)
  endif (EXISTS $ENV{TENSORFLOW_ROOT})
  find_library (TENSORFLOW_LIBRARY tensorflow.lib
                PATHS ${TENSORFLOW_ROOT}
                PATH_SUFFIXES lib
                DOC "searching for tensorflow.lib"
                NO_DEFAULT_PATH)
  if (NOT TENSORFLOW_LIBRARY)
   message (WARNING "could not find tensorflow.lib, continuing")
  else ()
   message (STATUS "Found tensorflow.lib library \"${TENSORFLOW_LIBRARY}\"")
  endif (NOT TENSORFLOW_LIBRARY)
  if (TENSORFLOW_LIBRARY)
   set (TENSORFLOW_FOUND TRUE)
   set (tensorflow_LIBRARIES "${TENSORFLOW_LIBRARY}")
   set (tensorflow_INCLUDE_DIRS "${TENSORFLOW_ROOT}/include")
   set (tensorflow_LIB_DIR "${TENSORFLOW_ROOT}/lib")
  endif (TENSORFLOW_LIBRARY)
 endif (NOT TENSORFLOW_FOUND)
 
 # if (EXISTS $ENV{TENSORFLOW_CC_ROOT})
 #  set (TENSORFLOW_CC_ROOT $ENV{TENSORFLOW_CC_ROOT})
 # else ()
 #  set (TENSORFLOW_CC_ROOT "C:/tf")
 # endif (EXISTS $ENV{TENSORFLOW_CC_ROOT})
 # find_library (TENSORFLOW_CC_LIBRARY libtensorflow_cc.so.2.15.0.if.lib
 #               PATHS ${TENSORFLOW_CC_ROOT}/bazel-out/x64_windows-opt/bin
 #               PATH_SUFFIXES tensorflow
 #               DOC "searching for libtensorflow_cc.so.2.15.0.if.lib"
 #               NO_DEFAULT_PATH)
#  if (NOT TENSORFLOW_CC_LIBRARY)
#   message (WARNING "could not find libtensorflow_cc.so.2.15.0.if.lib, continuing")
#  else ()
#   message (STATUS "Found libtensorflow_cc.so.2.15.0.if.lib library \"${TENSORFLOW_CC_LIBRARY}\"")
#  endif (NOT TENSORFLOW_CC_LIBRARY)
 find_library (TENSORFLOW_CC_LIBRARY tensorflow_cc.lib
               PATHS $ENV{LIB_ROOT}
               PATH_SUFFIXES tensorflow_cc
               DOC "searching for tensorflow_cc.lib"
               NO_DEFAULT_PATH)
 if (NOT TENSORFLOW_CC_LIBRARY)
  message (WARNING "could not find tensorflow_cc.lib, continuing")
 else ()
  message (STATUS "Found tensorflow_cc.lib library \"${TENSORFLOW_CC_LIBRARY}\"")
 endif (NOT TENSORFLOW_CC_LIBRARY)
 if (TENSORFLOW_CC_LIBRARY)
  set (TENSORFLOW_CC_FOUND TRUE)
  set (tensorflow_cc_LIBRARIES "${TENSORFLOW_CC_LIBRARY}")
  set (tensorflow_cc_INCLUDE_DIRS "$ENV{LIB_ROOT}/tensorflow_cc/include;$ENV{LIB_ROOT}/tensorflow_cc/include/src")
  set (tensorflow_cc_LIB_DIR "$ENV{LIB_ROOT}/tensorflow_cc")
 endif (TENSORFLOW_CC_LIBRARY)
endif ()
if (TENSORFLOW_FOUND)
 option (TENSORFLOW_SUPPORT "enable tensorflow support" ${TENSORFLOW_SUPPORT_DEFAULT})
 if (TENSORFLOW_SUPPORT)
  add_definitions (-DTENSORFLOW_SUPPORT)
#  include_directories (${tensorflow_INCLUDE_DIRS})
 endif (TENSORFLOW_SUPPORT)
endif (TENSORFLOW_FOUND)
if (TENSORFLOW_CC_FOUND)
 option (TENSORFLOW_CC_SUPPORT "enable tensorflow C++ support" ${TENSORFLOW_CC_SUPPORT_DEFAULT})
 if (TENSORFLOW_CC_SUPPORT)
  add_definitions (-DTENSORFLOW_CC_SUPPORT)
#  include_directories (${tensorflow_cc_INCLUDE_DIRS})
 endif (TENSORFLOW_CC_SUPPORT)
endif (TENSORFLOW_CC_FOUND)
