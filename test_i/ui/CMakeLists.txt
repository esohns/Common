include (${CMAKE_CURRENT_SOURCE_DIR}/../../src/ui/scripts/PreUIFramework.cmake)

if (GTK_SUPPORT)
 add_subdirectory (gtk)
endif (GTK_SUPPORT)

