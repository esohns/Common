include (libxml2)
include (ACEXML)

if (UNIX)
elseif (WIN32)
# set (MSXML_FOUND ON)
 set (MSXML_SUPPORT ON)
 add_definitions (-DMSXML_SUPPORT)
endif ()
