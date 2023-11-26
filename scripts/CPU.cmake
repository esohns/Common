if (UNIX)
 set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};$ENV{LIB_ROOT}/cmake-optimize-architecture-flag/Modules")
elseif (WIN32)
 set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}\;$ENV{LIB_ROOT}/cmake-optimize-architecture-flag/Modules")
 string (REPLACE "\\" "/" CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
endif ()
#message (STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
include (CMakeHostSystemInformationExtra)
include (GetCPUSIMDFeatures)
include (CMakeCompilerMachineOption)

cmake_host_system_information (RESULT HOST_CPU_HAS_SSE2 QUERY HAS_SSE2)
#cmake_host_system_information_extra (RESULT HOST_CPU_HAS_SSE2 QUERY HAS_SSE2)
if (HOST_CPU_HAS_SSE2)
 add_definitions (-DHOST_CPU_HAS_SSE2)
else ()
 message (STATUS "host system has no SSE2 support, continuing")
endif (HOST_CPU_HAS_SSE2)

cmake_host_system_information_extra (RESULT HOST_CPU_HAS_AVX2 QUERY HAS_AVX2)
if (${HOST_CPU_HAS_AVX2})
 add_definitions (-DHOST_CPU_HAS_AVX2)
else ()
 message (STATUS "host system has no AVX2 support, continuing")
endif (${HOST_CPU_HAS_AVX2})
