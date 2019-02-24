find_package (Threads REQUIRED)
find_package (ImageMagick COMPONENTS MagickCore MagickWand)

if (UNIX)
# find_library (SWSCALE_LIBRARY libswscale.so.5
#               DOC "searching for libswscale")
# if (NOT SWSCALE_LIBRARY)
#  message (FATAL_ERROR "could not find libswscale, aborting")
# endif (NOT SWSCALE_LIBRARY)
# find_library (AVFORMAT_LIBRARY libavformat.so.58
#               DOC "searching for libavformat")
# if (NOT AVFORMAT_LIBRARY)
#  message (FATAL_ERROR "could not find libavformat, aborting")
# endif (NOT AVFORMAT_LIBRARY)
# find_library (AVCODEC_LIBRARY libavcodec.so.58
#               DOC "searching for libavcodec")
# if (NOT AVCODEC_LIBRARY)
#  message (FATAL_ERROR "could not find libavcodec, aborting")
# endif (NOT AVCODEC_LIBRARY)
# pkg_check_modules (PKG_IMAGEMAGICK REQUIRED ImageMagick)
endif (UNIX)

set (ACE_LIB_FILE libACE.so)
if (DEFINED ENV{ACE_ROOT})
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES ace build/linux/ace
               DOC "searching for ${ACE_LIB_FILE}")
else ()
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               DOC "searching for ${ACE_LIB_FILE} (system paths)")
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
endif ()

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../.. REALPATH DIRECTORY)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)
find_package (Common ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
              REQUIRED COMPONENTS lib
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES libCommon/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${Common_DIR})
 message (FATAL_ERROR "could not find package \"Common\", aborting")
endif ()

############## definitions ###############
add_definitions (-DACE_HAS_DLL)
add_definitions (-DHAVE_CONFIG_H)
set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)
add_definitions (-DCOMMON_HAS_DLL)
add_definitions (-DCOMMON_TIMER_HAS_DLL)

############## includes ###############

if (UNIX)
# include_directories (${PKG_IMAGEMAGICK_INCLUDE_DIRS})
#include_directories (${ImageMagick_INCLUDE_DIRS})
 include_directories (/usr/include/ImageMagick-6/)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE)
endif (UNIX)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/log)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

############## next target ###############
set (imagemagick_EXE_SRCS
     test_i.cpp)

#cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property
set (EXECUTABLE imagemagick)
add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${imagemagick_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")

if (THREADS_HAVE_PTHREAD_ARG)
  set_property (TARGET ${EXECUTABLE} PROPERTY COMPILE_OPTIONS "-pthread")
  set_property (TARGET ${EXECUTABLE} PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif ()

if (CMAKE_THREAD_LIBS_INIT)
 target_link_libraries (${EXECUTABLE} "${CMAKE_THREAD_LIBS_INIT}")
endif ()
#target_link_libraries (${EXECUTABLE} ${PKG_IMAGEMAGICK_LIBRARIES})
target_link_libraries (${EXECUTABLE} ${ImageMagick_LIBRARIES})
target_link_libraries (${EXECUTABLE} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} Common)
target_link_libraries (${EXECUTABLE} CommonImage)
target_link_libraries (${EXECUTABLE} CommonLog)
target_link_libraries (${EXECUTABLE} CommonTimer)

# add PCH
#set_target_properties (${EXECUTABLE} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#cotire (${EXECUTABLE})

############### installation #############
install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)
# install (FILES etc/${EXECUTABLE}.glade
#          DESTINATION "${INSTALL_SHARE_DIR}/${PROJECT_NAME}/${EXECUTABLE}" COMPONENT config)

################ launchers ###############
create_target_launcher (${EXECUTABLE}
                        ARGS "12 /bin/bash"
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ATCD/ACE/lib
                                             ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../test_i/capabilities
                        ENVIRONMENT foo=bar)

################ tests ###################
add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE}
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\libCommon\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\libCommon\cmake\src\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\libCommon\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug

