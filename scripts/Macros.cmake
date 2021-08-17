
##########################################

if (WIN32)
macro (get_WIN32_Version version_major version_minor version_micro)
# sanity check(s)
 if (NOT CMAKE_SYSTEM_VERSION)
  message (FATAL_ERROR "CMAKE_SYSTEM_VERSION not set, aborting")
 endif (NOT CMAKE_SYSTEM_VERSION)

 message (STATUS "CMAKE_SYSTEM_VERSION: ${CMAKE_SYSTEM_VERSION}")
 set (ver ${CMAKE_SYSTEM_VERSION})
 string (REGEX REPLACE "^([0-9]+).([0-9]).([0-9]+)$" "\\1;\\2;\\3" result_list ${ver})
 list (LENGTH result_list n)
 list (GET result_list 0 ${version_major})
 if (NOT ${LENGTH} LESS 1)
  list (GET result_list 1 ${version_minor})
 endif ()
 if (NOT ${LENGTH} LESS 2)
  list (GET result_list 2 ${version_micro})
 endif ()
endmacro (get_WIN32_Version)

macro (get_WIN32_WINNT version)
# sanity check(s)
 if (NOT CMAKE_SYSTEM_VERSION)
  message (FATAL_ERROR "CMAKE_SYSTEM_VERSION not set, aborting")
 endif (NOT CMAKE_SYSTEM_VERSION)
 
 set (ver ${CMAKE_SYSTEM_VERSION})
#  message (STATUS "CMAKE_SYSTEM_VERSION: ${CMAKE_SYSTEM_VERSION}")
 string (REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
 string (REGEX MATCH "^([0-9]+)" verMajor ${ver})
 # Check for Windows 10, b/c we'll need to convert to hex 'A'.
 if ("${verMajor}" MATCHES "10")
  set (verMajor "A")
  string (REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
 endif ("${verMajor}" MATCHES "10")
 # Remove all remaining '.' characters.
 string (REPLACE "." "" ver ${ver})
 # Prepend each digit with a zero.
 string (REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
 set (${version} "0x${ver}")
endmacro (get_WIN32_WINNT)
endif (WIN32)

##########################################

if (MSVC)
macro (add_msvc_precompiled_header PrecompiledHeader PrecompiledSource SourcesVar)
 # set precompiled header binary name
 get_filename_component (PrecompiledBasename ${PrecompiledHeader} NAME_WE)
 set (PrecompiledBinary "$(IntDir)/${PrecompiledBasename}.pch")

 # generate the precompiled header
 set_source_files_properties (${PrecompiledSource} PROPERTIES
                              COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                              OBJECT_OUTPUTS "${PrecompiledBinary}")

 # set the usage of this header only to the other files than rc
 set (Sources ${${SourcesVar}})
 foreach (fname ${Sources})
  if (NOT ${fname} MATCHES ".*rc$")
   set_source_files_properties (${fname} PROPERTIES
                                COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                OBJECT_DEPENDS "${PrecompiledBinary}")
  endif (NOT ${fname} MATCHES ".*rc$")
 endforeach (fname)

 # add precompiled header to SourcesVar
 list (APPEND ${SourcesVar} ${PrecompiledSource})
endmacro (add_msvc_precompiled_header)
endif (MSVC)

##########################################

macro (is_UI_graphical UI)
# sanity check(s)
 if (NOT GUI_SUPPORT)
  message (STATUS "GUI_SUPPORT not set, aborting")
 endif (NOT GUI_SUPPORT)


 set (${UI} FALSE)
endmacro (is_UI_graphical)

##########################################

if (LINUX)
function (get_linux_lsb_release_information)
 find_program (LSB_RELEASE_EXEC lsb_release)
 if (NOT LSB_RELEASE_EXEC)
  message (FATAL_ERROR "Could not detect lsb_release executable, can not gather required information")
 endif (NOT LSB_RELEASE_EXEC)

 execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --id OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
 execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --release OUTPUT_VARIABLE LSB_RELEASE_VERSION_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
 execute_process(COMMAND "${LSB_RELEASE_EXEC}" --short --codename OUTPUT_VARIABLE LSB_RELEASE_CODENAME_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)

 set (LSB_RELEASE_ID_SHORT "${LSB_RELEASE_ID_SHORT}" PARENT_SCOPE)
 set (LSB_RELEASE_VERSION_SHORT "${LSB_RELEASE_VERSION_SHORT}" PARENT_SCOPE)
 set (LSB_RELEASE_CODENAME_SHORT "${LSB_RELEASE_CODENAME_SHORT}" PARENT_SCOPE)
endfunction (get_linux_lsb_release_information)
endif (LINUX)

