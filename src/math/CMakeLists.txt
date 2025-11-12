set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/scripts")
include (fftw)

set (LIBRARY ${PROJECT_NAME}Math)
add_library (${LIBRARY} STATIC "")

if (FFTW_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${FFTW_INCLUDE_DIRS}>)
endif (FFTW_SUPPORT)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../timer>)

if (WIN32)
 add_definitions (-D_USE_MATH_DEFINES)
 if (FFTW_SUPPORT)
  add_definitions (-D_CRT_USE_C_COMPLEX_H)
#  add_definitions (-DFFTW_DLL)
#  add_definitions (-Dcomplex) *TODO*: using C99 complex.h does not seem to work
 endif (FFTW_SUPPORT)
endif (WIN32)

############## next target ################
set (source_files
     common_math_defines.h
     common_math_fft.cpp
     common_math_fft.h
     common_math_fft.inl
     common_math_pidcontrol.cpp
     common_math_pidcontrol.h
     common_math_pidcontrol_zieglernichols.cpp
     common_math_pidcontrol_zieglernichols.h
     common_math_pt1_tracker.cpp
     common_math_pt1_tracker.h
     common_math_pt1_tracker.inl
     common_math_sample.cpp
     common_math_sample.h
     common_math_sample.inl
     common_math_tools.cpp
     common_math_tools.h
     common_math_tools.inl)
if (FFTW_SUPPORT)
 set (source_files
      ${source_files}
     common_math_fftw.cpp
     common_math_fftw.h
     common_math_fftw.inl)
endif (FFTW_SUPPORT)

# Include dir for installed targets
#set (INCLUDE_DIR include/${PROJECT_NAME}-${upstream_version})

#set_target_properties (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

unset (absolute_source_files)
foreach (source_file ${source_files})
 get_filename_component (abs_source_file ${source_file} ABSOLUTE)
 list (APPEND absolute_source_files
       PRIVATE ${abs_source_file}
       )
#       $<INSTALL_INTERFACE:${INCLUDE_DIR}/${source_file}>)
endforeach ()
target_sources (${LIBRARY}
#                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                PRIVATE ${absolute_source_files})

if (WIN32)
 if (LLVM_USE)
#  target_compile_options (${LIBRARY} PUBLIC -Wno-c++11-narrowing)
 endif (LLVM_USE)
endif (WIN32)

#*TODO*: use pkg-config instead
if (WIN32)
# target_link_libraries (${LIBRARY} ${COMMON_TARGET_NAME})
 target_link_libraries (${LIBRARY} ${ACE_LIBRARY})
endif (WIN32)

#set_target_properties (${LIBRARY} PROPERTIES DEFINE_SYMBOL COMMON_MATH_BUILD_DLL)
#set_target_properties (${LIBRARY} PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

set_property (TARGET ${LIBRARY} PROPERTY FOLDER "modules")

# add all targets to the build-tree export set
export (TARGETS ${LIBRARY}
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (header_files
     ${header_files}
     common_math_defines.h
     common_math_fft.h
     common_math_fft.inl
     common_math_pidcontrol.h
     common_math_pidcontrol_zieglernichols.h
     common_math_pt1_tracker.h
     common_math_pt1_tracker.inl
     common_math_sample.h
     common_math_sample.inl
     common_math_tools.h
     common_math_tools.inl)
if (FFTW_SUPPORT)
 set (header_files
      ${header_files}
      common_math_fftw.h
      common_math_fftw.inl)
endif (FFTW_SUPPORT)
install (FILES
         ${header_files}
         DESTINATION include)
install (TARGETS ${LIBRARY}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)
