/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef COMMON_UI_COMMON_H
#define COMMON_UI_COMMON_H

#include <cstdint>

#include <limits>
#include <list>
#include <string>
#include <vector>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "windef.h"
//#include "WinUser.h"
//#include "wtypes.h"
#else
//#include "X11/Xlib.h"
// forward declarations
struct _XDisplay;
typedef struct {
    short x, y;
    unsigned short width, height;
} Common_UI_Rectangle;
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Condition_Thread_Mutex.h"
#include "ace/Containers_T.h"
#include "ace/OS.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Thread_Mutex.h"

#include "common_log_common.h"

// #######################################

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef struct tagSIZE Common_UI_Resolution_t;
struct common_ui_resolution_less
{
  inline bool operator() (const Common_UI_Resolution_t& lhs_in, const Common_UI_Resolution_t& rhs_in) const { return ((lhs_in.cx == rhs_in.cx) ? (lhs_in.cy < rhs_in.cy) : (lhs_in.cx < rhs_in.cx)); }
};
struct common_ui_resolution_equal
{
  inline bool operator() (const Common_UI_Resolution_t& lhs_in, const Common_UI_Resolution_t& rhs_in) const { return ((lhs_in.cx == rhs_in.cx) && (lhs_in.cy == rhs_in.cy)); }
};
#else
struct Common_UI_Resolution
{
  unsigned int width;
  unsigned int height;
};
typedef struct Common_UI_Resolution Common_UI_Resolution_t;
struct common_ui_resolution_less
{
  inline bool operator() (const Common_UI_Resolution_t& lhs_in, const Common_UI_Resolution_t& rhs_in) const { return ((lhs_in.width == rhs_in.width) ? (lhs_in.height < rhs_in.height) : (lhs_in.width < rhs_in.width)); }
};
struct common_ui_resolution_equal
{
  inline bool operator() (const Common_UI_Resolution_t& lhs_in, const Common_UI_Resolution_t& rhs_in) const { return ((lhs_in.width == rhs_in.width) && (lhs_in.height == rhs_in.height)); }
};
#endif // ACE_WIN32 || ACE_WIN64
typedef std::list<Common_UI_Resolution_t> Common_UI_Resolutions_t;
typedef Common_UI_Resolutions_t::iterator Common_UI_ResolutionsIterator_t;
typedef Common_UI_Resolutions_t::const_iterator Common_UI_ResolutionsConstIterator_t;

// graphics
typedef std::vector<unsigned int> Common_UI_Framerates_t;
typedef Common_UI_Framerates_t::iterator Common_UI_FrameratesIterator_t;
typedef Common_UI_Framerates_t::const_iterator Common_UI_FrameratesConstIterator_t;

// #######################################

// adapters (i.e. 'graphics' cards/chips)
struct Common_UI_DisplayAdapterHead
{
  Common_UI_DisplayAdapterHead ()
   : device ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , index (std::numeric_limits<DWORD>::max ())
   , key ()
#endif // ACE_WIN32 || ACE_WIN64
  {}

  std::string device; // display-
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  DWORD       index;  // iDevNum
  std::string key;    // registry-
#endif // ACE_WIN32 || ACE_WIN64
};
typedef std::list<struct Common_UI_DisplayAdapterHead> Common_UI_DisplayAdapterHeads_t;
typedef Common_UI_DisplayAdapterHeads_t::iterator Common_UI_DisplayAdapterHeadsIterator_t;
typedef Common_UI_DisplayAdapterHeads_t::const_iterator Common_UI_DisplayAdapterHeadsConstIterator_t;
struct Common_UI_DisplayAdapter
{
  Common_UI_DisplayAdapter ()
   : clippingArea ()
   , description ()
   , device ()
   , heads ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
   , driver ()
   , slot () // PCI-
#endif // ACE_WIN32 || ACE_WIN64
  {}
  inline bool operator== (const struct Common_UI_DisplayAdapter& rhs_in) { return !ACE_OS::strcmp (description.c_str (), rhs_in.description.c_str ()); }

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct tagRECT                  clippingArea;
#else
  Common_UI_Rectangle             clippingArea;
#endif // ACE_WIN32 || ACE_WIN64
  std::string                     description;
  std::string                     device;
  Common_UI_DisplayAdapterHeads_t heads;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  std::string                     driver;
  std::string                     slot;
#endif // ACE_WIN32 || ACE_WIN64
};
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//struct common_ui_adapter_equal
//{
//  inline bool operator() (const struct Common_UI_DisplayAdapter& lhs_in, const struct Common_UI_DisplayAdapter& rhs_in) const { return !ACE_OS::strcmp (lhs_in.id.c_str (), rhs_in.id.c_str ()); }
//};
//#endif // ACE_WIN32 || ACE_WIN64
typedef std::list<struct Common_UI_DisplayAdapter> Common_UI_DisplayAdapters_t;
typedef Common_UI_DisplayAdapters_t::iterator Common_UI_DisplayAdaptersIterator_t;
//typedef Common_UI_DisplayAdapters_t::const_iterator Common_UI_DisplayAdaptersConstIterator_t;

// device ('physical'- i.e. screens, monitors, etc.)
struct Common_UI_DisplayDevice
{
  Common_UI_DisplayDevice ()
   : clippingArea ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , handle (NULL)
   , id ()
   , key ()
#endif // ACE_WIN32 || ACE_WIN64
   , description ()
   , device ()
   , primary (false)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    //BOOL result = SetRectEmpty (&clippingArea);
    //ACE_ASSERT (result);
    ACE_OS::memset (&clippingArea, 0, sizeof (struct tagRECT));
#else
    ACE_OS::memset (&clippingArea, 0, sizeof (Common_UI_Rectangle));
#endif // ACE_WIN32 || ACE_WIN64
  }

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct tagRECT      clippingArea;
  HMONITOR            handle;
  std::string         id;
  std::string         key;
#else
  Common_UI_Rectangle clippingArea;
#endif // ACE_WIN32 || ACE_WIN64
  std::string         description;
  std::string         device;
  bool                primary;
};
typedef std::list<struct Common_UI_DisplayDevice> Common_UI_DisplayDevices_t;
typedef Common_UI_DisplayDevices_t::iterator Common_UI_DisplayDevicesIterator_t;
//typedef Common_UI_DisplayDevices_t::const_iterator Common_UI_DisplayDevicesConstIterator_t;

// device ('logical'-)
struct Common_UI_Display
{
  Common_UI_Display ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
   : device () // output device (i.e. monitor description)
   , display (NULL)
#endif // ACE_WIN32 || ACE_WIN64
  {}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  std::string       device;
  struct _XDisplay* display;
#endif // ACE_WIN32 || ACE_WIN64
};

struct Common_UI_DisplayMode
{
  Common_UI_DisplayMode ()
   : bitsperpixel (0)
   , frequency (0)
   , fullscreen (false)
   , resolution ()
  {}

  uint8_t                bitsperpixel;
  uint8_t                frequency;
  bool                   fullscreen;
  Common_UI_Resolution_t resolution;
};

// #######################################

// ui
enum Common_UI_FrameworkType
{
  COMMON_UI_FRAMEWORK_INVALID = -1,

  COMMON_UI_FRAMEWORK_CURSES = 0,
  COMMON_UI_FRAMEWORK_GTK,
  COMMON_UI_FRAMEWORK_QT,
  COMMON_UI_FRAMEWORK_WXWIDGETS,
  // -------------------------------------
  COMMON_UI_FRAMEWORK_MAX
};

enum Common_UI_EventType
{
  COMMON_UI_EVENT_INVALID              = -1,

  // message & data
  COMMON_UI_EVENT_MESSAGE_DATA_BASE    = 0x0100,
  COMMON_UI_EVENT_CONTROL,
  COMMON_UI_EVENT_DATA,
  COMMON_UI_EVENT_SESSION,

  // task
  COMMON_UI_EVENT_TASK_BASE            = 0x0200,
  COMMON_UI_EVENT_FINISHED,
  COMMON_UI_EVENT_PAUSED,
  COMMON_UI_EVENT_RESET,
  COMMON_UI_EVENT_STARTED,
  COMMON_UI_EVENT_STOPPED,

  // network & session
  COMMON_UI_EVENT_NETWORK_SESSION_BASE = 0x0400,
  COMMON_UI_EVENT_ABORT,
  COMMON_UI_EVENT_CONNECT,
  COMMON_UI_EVENT_DISCONNECT,
  ////////////////////////////////////////
  COMMON_UI_EVENT_STATISTIC,

  // UI
  COMMON_UI_EVENT_UI_BASE              = 0x0800,
  COMMON_UI_EVENT_RESIZE,

  // other & user
  COMMON_UI_EVENT_OTHER_USER_BASE      = 0x1000,

  // -------------------------------------
  COMMON_UI_EVENT_MAX
};
typedef ACE_Unbounded_Stack<enum Common_UI_EventType> Common_UI_Events_t;
typedef ACE_Unbounded_Stack<enum Common_UI_EventType>::ITERATOR Common_UI_EventsIterator_t;

struct Common_UI_State
{
  Common_UI_State ()
   : condition (lock)
   , eventStack (NULL)
   , lock ()
   , logStack ()
   , logStackLock ()
   , subscribersLock ()
  {}

  ACE_SYNCH_CONDITION       condition;
  Common_UI_Events_t        eventStack;
  ACE_SYNCH_MUTEX           lock;
  Common_MessageStack_t     logStack;
  ACE_SYNCH_MUTEX           logStackLock;
  ACE_SYNCH_RECURSIVE_MUTEX subscribersLock;
};

//////////////////////////////////////////

struct Common_UI_ProgressData
{
  Common_UI_ProgressData ()
   : state (NULL)
  {}

  Common_UI_State* state;
};

struct Common_UI_CBData
{
  Common_UI_CBData ()
   : UIState (NULL)
  {}

  struct Common_UI_State* UIState;
};

#endif
