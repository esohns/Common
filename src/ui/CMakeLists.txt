option (GTK_SUPPORT "enable GTK support" ON)

############## next target ################
set (${PROJECT_NAME}UI_LIB_SRCS
     stdafx.cpp
     stdafx.h
     common_ui_common.h
     common_ui_defines.h
     common_ui_exports.h
     common_ui_tools.cpp
     common_ui_tools.h
     common_ui_types.h)

add_library (${PROJECT_NAME}UI
             STATIC
             ${CommonUI_LIB_SRCS})

add_dependencies (${PROJECT_NAME}UI Common)
if (UNIX)
elseif (WIN32)
# target_link_libraries (${PROJECT_NAME}UI Common)
endif ()

if (WIN32)
# set_target_properties (${PROJECT_NAME}UI PROPERTIES LINK_FLAGS /SAFESEH:NO)
endif ()
set_target_properties (${PROJECT_NAME}UI PROPERTIES DEFINE_SYMBOL COMMON_UI_BUILD_DLL)
set_target_properties (${PROJECT_NAME}UI PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add PCH
set_target_properties (${PROJECT_NAME}UI PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME}UI)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}UI
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
set (${PROJECT_NAME}UI_LIB_HDRS
     common_ui_common.h
     common_ui_defines.h
     common_ui_exports.h
     common_ui_tools.h
     common_ui_types.h)

install (FILES
         ${CommonUI_LIB_HDRS}
         DESTINATION include)
install (TARGETS ${PROJECT_NAME}UI
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
         COMPONENT lib)

if (GTK_SUPPORT)
 add_subdirectory (gtk)
endif ()
