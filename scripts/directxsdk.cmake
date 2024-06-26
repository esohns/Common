if (WIN32)
 if (VCPKG_USE)
#  find_package (DIRECTXSDK CONFIG)
  find_library (D3DX9_LIBRARY d3dx9${LIB_FILE_SUFFIX}.lib
                PATHS ${VCPKG_LIB_DIR}
                DOC "searching for d3dx9${LIB_FILE_SUFFIX}.lib"
                NO_DEFAULT_PATH)
  if (D3DX9_LIBRARY)
   message (STATUS "found d3dx9${LIB_FILE_SUFFIX}.lib library \"${D3DX9_LIBRARY}\"")
   set (DIRECTXSDK_FOUND TRUE)
   set (DIRECTXSDK_INCLUDE_DIR ${VCPKG_INCLUDE_DIR}/directxsdk)
   set (DIRECTXSDK_LIBRARIES ${D3DX9_LIBRARY})
   set (DIRECTXSDK_LIB_DIR "${VCPKG_BIN_DIR}")
  endif (D3DX9_LIBRARY)
 endif (VCPKG_USE)
 if (NOT DIRECTXSDK_FOUND)
  if (DEFINED ENV{DXSDK_DIR})
   set (DXSDK_ROOT $ENV{DXSDK_DIR})
  elseif (DEFINED ENV{LIB_ROOT})
   set (DXSDK_ROOT $ENV{LIB_ROOT}/DXSDK)
  else ()
   message (FATAL_ERROR "DirectX SDK not found, aborting")
  endif ()
  find_library (D3DX9_LIBRARY d3dx9.lib
                PATHS ${DXSDK_ROOT}/Lib
                PATH_SUFFIXES ${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}
                DOC "searching for d3dx9.lib"
                NO_DEFAULT_PATH)
  if (NOT D3DX9_LIBRARY)
   message (FATAL_ERROR "could not find d3dx9.lib, continuing")
  else ()
   message (STATUS "found d3dx9.lib library \"${D3DX9_LIBRARY}\"")
  endif (NOT D3DX9_LIBRARY)
  if (D3DX9_LIBRARY)
   set (DIRECTXSDK_FOUND TRUE)
   set (DIRECTXSDK_LIBRARIES ${D3DX9_LIBRARY})
   set (DIRECTXSDK_INCLUDE_DIR "${DXSDK_ROOT}/Include")
   set (DIRECTXSDK_LIB_DIR "${DXSDK_ROOT}/Developer Runtime/${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}")
  endif (D3DX9_LIBRARY)
 endif (NOT DIRECTXSDK_FOUND)
endif (WIN32)
if (DIRECTXSDK_FOUND)
 option (DIRECTXSDK_SUPPORT "enable DirectX SDK support" ON)
 if (DIRECTXSDK_SUPPORT)
  add_definitions (-DDIRECTXSDK_SUPPORT)
 endif (DIRECTXSDK_SUPPORT)
endif (DIRECTXSDK_FOUND)
