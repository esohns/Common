include (${CMAKE_CURRENT_SOURCE_DIR}/../../src/ui/scripts/PreUIFramework.cmake)

#########################################

if (GTK_SUPPORT)
 add_subdirectory (gtk)
endif (GTK_SUPPORT)
if (QT_SUPPORT)
 add_subdirectory (qt)
endif (QT_SUPPORT)
if (WXWIDGETS_SUPPORT)
 add_subdirectory (wxwidgets)
endif (WXWIDGETS_SUPPORT)
