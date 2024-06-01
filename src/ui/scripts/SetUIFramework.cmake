# *NOTE*: the first entry is the default option
set (UI_FRAMEWORK "none" CACHE STRING "none (default)")
set (SUPPORTED_UI_FRAMEWORKS "none")
if (GTK_SUPPORT)
 set (SUPPORTED_UI_FRAMEWORKS "${SUPPORTED_UI_FRAMEWORKS};gtk")
endif (GTK_SUPPORT)
if (QT_SUPPORT)
 set (SUPPORTED_UI_FRAMEWORKS "${SUPPORTED_UI_FRAMEWORKS};qt")
endif (QT_SUPPORT)
if (WXWIDGETS_SUPPORT)
 set (SUPPORTED_UI_FRAMEWORKS "${SUPPORTED_UI_FRAMEWORKS};wxwidgets")
endif (WXWIDGETS_SUPPORT)
if (CURSES_SUPPORT)
 set (SUPPORTED_UI_FRAMEWORKS "${SUPPORTED_UI_FRAMEWORKS};curses")
endif (CURSES_SUPPORT)
set_property (CACHE UI_FRAMEWORK PROPERTY STRINGS ${SUPPORTED_UI_FRAMEWORKS})

if (NOT DEFINED UI_FRAMEWORK_LAST)
 set (UI_FRAMEWORK_LAST "NotAnImplementation" CACHE STRING "last user interface implementation used")
 mark_as_advanced (FORCE UI_FRAMEWORK_LAST)
endif (NOT DEFINED UI_FRAMEWORK_LAST)
if (NOT (${UI_FRAMEWORK} MATCHES ${UI_FRAMEWORK_LAST}))
 unset (GTK_USE CACHE)
 unset (QT_USE CACHE)
 unset (WXWIDGETS_USE CACHE)
 unset (CURSES_USE CACHE)
 set (UI_FRAMEWORK_LAST ${UI_FRAMEWORK} CACHE STRING "Updating User Interface Implementation Option" FORCE)
endif (NOT (${UI_FRAMEWORK} MATCHES ${UI_FRAMEWORK_LAST}))

if (${UI_FRAMEWORK} MATCHES "gtk")
 if (NOT GTK_SUPPORT)
  message (WARNING "GTK not supported")
 else ()
  set (GTK_USE ON CACHE STRING "use GTK")
  mark_as_advanced (FORCE GTK_USE)
  add_definitions (-DGTK_USE)
 endif (NOT GTK_SUPPORT)
elseif (${UI_FRAMEWORK} MATCHES "qt")
 if (NOT QT_SUPPORT)
  message (WARNING "Qt not supported")
 else ()
  set (QT_USE ON CACHE STRING "use Qt")
  mark_as_advanced (FORCE QT_USE)
  add_definitions (-DQT_USE)
 endif (NOT QT_SUPPORT)
elseif (${UI_FRAMEWORK} MATCHES "wxWidgets")
 if (NOT WXWIDGETS_FOUND)
  message (WARNING "wxWidgets not supported")
 else ()
  set (WXWIDGETS_USE ON CACHE STRING "use wxWidgets")
  mark_as_advanced (FORCE WXWIDGETS_USE)
  add_definitions (-DWXWIDGETS_USE)
 endif (NOT WXWIDGETS_FOUND)
elseif (${UI_FRAMEWORK} MATCHES "curses")
 if (NOT CURSES_FOUND)
  message (WARNING "curses not supported")
 else ()
  set (CURSES_USE ON CACHE STRING "use curses")
  mark_as_advanced (FORCE CURSES_USE)
  add_definitions (-DCURSES_USE)
 endif (NOT CURSES_FOUND)
endif ()
