# *NOTE*: the first entry is the default option
set (SUPPORTED_XML_IMPLEMENTATIONS "")
if (UNIX)
 if (LIBXML2_SUPPORT)
  set (SUPPORTED_XML_IMPLEMENTATIONS "libxml2")
  set (XML_IMPLEMENTATION "libxml2" CACHE STRING "libxml2 (default)")
 endif (LIBXML2_SUPPORT)
 if (ACEXML_SUPPORT)
  if (LIBXML2_SUPPORT)
   set (SUPPORTED_XML_IMPLEMENTATIONS "${SUPPORTED_XML_IMPLEMENTATIONS};ACEXML")
  else ()
   set (SUPPORTED_XML_IMPLEMENTATIONS "ACEXML")
   set (XML_IMPLEMENTATION "ACEXML" CACHE STRING "ACEXML (default)")
  endif (LIBXML2_SUPPORT)
 endif (ACEXML_SUPPORT)
elseif (WIN32)
 set (SUPPORTED_XML_IMPLEMENTATIONS "msxml")
 set (XML_IMPLEMENTATION "msxml" CACHE STRING "msxml (default)")
 if (LIBXML2_SUPPORT)
  set (SUPPORTED_XML_IMPLEMENTATIONS "${SUPPORTED_XML_IMPLEMENTATIONS};libxml2")
 endif (LIBXML2_SUPPORT)
 if (ACEXML_SUPPORT)
  set (SUPPORTED_XML_IMPLEMENTATIONS "${SUPPORTED_XML_IMPLEMENTATIONS};ACEXML")
 endif (ACEXML_SUPPORT)
endif ()
set_property (CACHE XML_IMPLEMENTATION PROPERTY STRINGS ${SUPPORTED_XML_IMPLEMENTATIONS})

if (NOT DEFINED XML_IMPLEMENTATION_LAST)
 set (XML_IMPLEMENTATION_LAST "NotAnImplementation" CACHE STRING "last XML implementation used")
 mark_as_advanced (FORCE XML_IMPLEMENTATION_LAST)
endif (NOT DEFINED XML_IMPLEMENTATION_LAST)
if (NOT (${XML_IMPLEMENTATION} MATCHES ${XML_IMPLEMENTATION_LAST}))
 if (UNIX)
  unset (LIBXML2_USE CACHE)
  unset (ACEXML_USE CACHE)
 elseif (WIN32)
  unset (MSXML_USE CACHE)
  unset (LIBXML2_USE CACHE)
  unset (ACEXML_USE CACHE)
 endif ()
  set (XML_IMPLEMENTATION_LAST ${XML_IMPLEMENTATION} CACHE STRING "Updating XML Implementation Option" FORCE)
endif (NOT (${XML_IMPLEMENTATION} MATCHES ${XML_IMPLEMENTATION_LAST}))

if (UNIX)
 if (${XML_IMPLEMENTATION} MATCHES "libxml2")
  if (NOT LIBXML2_SUPPORT)
   message (FATAL_ERROR "libxml2 not supported")
  endif (NOT LIBXML2_SUPPORT)
  set (LIBXML2_USE ON CACHE STRING "use libxml2")
  mark_as_advanced (FORCE LIBXML2_USE)
  add_definitions (-DLIBXML2_USE)
  set (XML_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIRS})
  set (XML_LIBRARIES ${LIBXML2_LIBRARIES})
  set (XML_LIB_DIRS ${LIBXML2_LIB_DIRS})
 elseif (${XML_IMPLEMENTATION} MATCHES "ACEXML")
  if (NOT ACEXML_SUPPORT)
   message (FATAL_ERROR "ACEXML not supported")
  endif (NOT ACEXML_SUPPORT)
  set (ACEXML_USE ON CACHE STRING "use ACEXML")
  mark_as_advanced (FORCE ACEXML_USE)
  add_definitions (-DACEXML_USE)
  set (XML_INCLUDE_DIRS ${ACE_INCLUDE_DIRS})
  set (XML_LIBRARIES ${ACEXML_LIBRARIES})
  set (XML_LIB_DIRS ${ACE_LIB_DIRS})
 endif ()
elseif (WIN32)
 if (${XML_IMPLEMENTATION} MATCHES "msxml")
  if (NOT MSXML_SUPPORT)
   message (FATAL_ERROR "msxml not supported")
  endif (NOT MSXML_SUPPORT)
  set (MSXML_USE ON)
  mark_as_advanced (FORCE MSXML_USE)
  add_definitions (-DMSXML_USE)
 elseif (${XML_IMPLEMENTATION} MATCHES "libxml2")
  if (NOT LIBXML2_SUPPORT)
   message (FATAL_ERROR "libxml2 not supported")
  endif (NOT LIBXML2_SUPPORT)
  set (LIBXML2_USE ON CACHE STRING "use libxml2")
  mark_as_advanced (FORCE LIBXML2_USE)
  add_definitions (-DLIBXML2_USE)
  set (XML_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIRS})
  set (XML_LIBRARIES ${LIBXML2_LIBRARIES})
  set (XML_LIB_DIRS ${LIBXML2_LIB_DIRS})
 elseif (${XML_IMPLEMENTATION} MATCHES "ACEXML")
  if (NOT ACEXML_SUPPORT)
   message (FATAL_ERROR "ACEXML not supported")
  endif (NOT ACEXML_SUPPORT)
  set (ACEXML_USE ON CACHE STRING "use ACEXML")
  mark_as_advanced (FORCE ACEXML_USE)
  add_definitions (-DACEXML_USE)
  set (XML_INCLUDE_DIRS ${ACE_INCLUDE_DIRS})
  set (XML_LIBRARIES ${ACEXML_LIBRARIES})
  set (XML_LIB_DIRS ${ACE_LIB_DIRS})
 endif ()
endif ()
