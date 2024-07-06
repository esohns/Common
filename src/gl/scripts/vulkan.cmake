if (UNIX)
 set (VULKAN_LIB_FILE libVulkan.so)
 find_library (VULKAN_LIBRARY ${VULKAN_LIB_FILE}
               PATHS /usr /usr/lib
               PATH_SUFFIXES lib64 x86_64-linux-gnu
               DOC "searching for ${VULKAN_LIB_FILE}")
 if (VULKAN_LIBRARY)
  message (STATUS "Found Vulkan library \"${VULKAN_LIBRARY}\"")
  set (VULKAN_FOUND TRUE)
#  set (VULKAN_INCLUDE_DIR "/usr/include")
  set (VULKAN_LIBRARIES "${VULKAN_LIBRARY}")
 else ()
  message (WARNING "could not find ${VULKAN_LIB_FILE}, continuing")
 endif (VULKAN_LIBRARY)
elseif (WIN32)
 if (VCPKG_USE)
  find_package (Vulkan)
  if (VULKAN_FOUND)
   message (STATUS "found Vulkan")
   set (VULKAN_INCLUDE_DIRS ${VCPKG_INCLUDE_DIR})
   set (VULKAN_LIBRARIES "Vulkan::Vulkan")
   set (VULKAN_LIB_DIR "${VCPKG_BIN_DIR}")
  endif (VULKAN_FOUND)
 endif (VCPKG_USE)
 if (NOT VULKAN_FOUND)
  set (VULKAN_LIB_FILE vulkan-1.lib)
  set (BIN_PATH_BASE "Bin")
  set (LIB_PATH_BASE "Lib")
  find_library (VULKAN_LIBRARY ${VULKAN_LIB_FILE}
                PATHS $ENV{LIB_ROOT}/vulkan
                PATH_SUFFIXES ${LIB_PATH_BASE}
                DOC "searching for ${VULKAN_LIB_FILE}")
  if (VULKAN_LIBRARY)
   message (STATUS "found Vulkan library \"${VULKAN_LIBRARY}\"")
  set (VULKAN_FOUND TRUE)
  set (VULKAN_INCLUDE_DIRS "$ENV{LIB_ROOT}/vulkan/Include")
  set (VULKAN_LIBRARIES ${VULKAN_LIBRARY})
  set (VULKAN_LIB_DIR "$ENV{LIB_ROOT}/${BIN_PATH_BASE}")
  else ()
   message (WARNING "could not find ${VULKAN_LIB_FILE}, continuing")
  endif (VULKAN_LIBRARY)
 endif (NOT VULKAN_FOUND)
endif ()
if (VULKAN_FOUND)
 option (VULKAN_SUPPORT "enable Vulkan support" ON)
 if (VULKAN_SUPPORT)
  add_definitions (-DVULKAN_SUPPORT)
 endif (VULKAN_SUPPORT)
endif (VULKAN_FOUND)
