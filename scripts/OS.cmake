##########################################

function (get_linux_lsb_release_information)
 find_program (LSB_RELEASE_EXEC lsb_release
               PATHS /usr
               PATH_SUFFIXES bin)
 if (NOT LSB_RELEASE_EXEC)
  message (FATAL_ERROR "could not detect lsb_release executable, can not gather required information")
 endif (NOT LSB_RELEASE_EXEC)

 execute_process (COMMAND "${LSB_RELEASE_EXEC}" --short --id OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
 execute_process (COMMAND "${LSB_RELEASE_EXEC}" --short --release OUTPUT_VARIABLE LSB_RELEASE_VERSION_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
 execute_process (COMMAND "${LSB_RELEASE_EXEC}" --short --codename OUTPUT_VARIABLE LSB_RELEASE_CODENAME_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)

 set (LSB_RELEASE_ID_SHORT "${LSB_RELEASE_ID_SHORT}" PARENT_SCOPE)
 set (LSB_RELEASE_VERSION_SHORT "${LSB_RELEASE_VERSION_SHORT}" PARENT_SCOPE)
 set (LSB_RELEASE_CODENAME_SHORT "${LSB_RELEASE_CODENAME_SHORT}" PARENT_SCOPE)
endfunction (get_linux_lsb_release_information)

##########################################

if (UNIX)
 if (CMAKE_SYSTEM_NAME MATCHES "Linux")
  get_linux_lsb_release_information ()
  message (STATUS "Linux ${LSB_RELEASE_ID_SHORT} ${LSB_RELEASE_VERSION_SHORT} ${LSB_RELEASE_CODENAME_SHORT}")
  if (${LSB_RELEASE_ID_SHORT} MATCHES "Debian")
   add_definitions (-DIS_DEBIAN_LINUX)
  elseif (${LSB_RELEASE_ID_SHORT} MATCHES "Fedora")
   add_definitions (-DIS_FEDORA_LINUX)
  elseif (${LSB_RELEASE_ID_SHORT} MATCHES "Ubuntu")
   add_definitions (-DIS_UBUNTU_LINUX)
  endif ()
 endif (CMAKE_SYSTEM_NAME MATCHES "Linux")
endif (UNIX)
