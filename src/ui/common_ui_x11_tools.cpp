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
#include "stdafx.h"

#include "common_ui_x11_tools.h"

#include "X11/Xlib.h"
#include "X11/extensions/Xrandr.h"

#include "common_ui_defines.h"
#include "common_ui_monitor_setup_xml_handler.h"

#include "common_xml_defines.h"

std::string
Common_UI_X11_Tools::getX11DisplayName (const std::string& outputName_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_X11_Tools::getX11DisplayName"));

  const char* display_p =
      ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_X11_DISPLAY_ENVIRONMENT_VARIABLE));
  if (!display_p)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("environment \"%s\" not set, using default display, continuing\n"),
                ACE_TEXT (COMMON_UI_X11_DISPLAY_ENVIRONMENT_VARIABLE)));

  int result = -1;
  struct _XDisplay* display_2 = XOpenDisplay (display_p);
  if (!display_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XOpenDisplay(%s), aborting\n"),
                (display_p ? ACE_TEXT (display_p) : ACE_TEXT ("NULL"))));
    return ACE_TEXT_ALWAYS_CHAR ("");
  } // end IF

  // verify that the output is being used by the X11 session
//  Screen* screen_p = NULL;
  XRRScreenResources* screen_resources_p = NULL;
  XRROutputInfo* output_info_p = NULL;
  XRRCrtcInfo* crtc_info_p = NULL;
  for (int i = 0;
       i < ScreenCount (display_2);
       ++i)
  {
//    screen_p = ScreenOfDisplay (display_2, i);
//    ACE_ASSERT (screen_p); ACE_UNUSED_ARG (screen_p);

    screen_resources_p =
        XRRGetScreenResources (display_2,
                               RootWindow (display_2, i));
    ACE_ASSERT (screen_resources_p);
    for (int j = 0;
         j < screen_resources_p->noutput;
         ++j)
    {
      output_info_p = XRRGetOutputInfo (display_2,
                                        screen_resources_p,
                                        screen_resources_p->outputs[j]);
      ACE_ASSERT (output_info_p);
      if ((output_info_p->connection != RR_Connected) ||
          ACE_OS::strcmp (output_info_p->name,
                          outputName_in.c_str ()))
      {
        XRRFreeOutputInfo (output_info_p); output_info_p = NULL;
        continue;
      } // end IF
      crtc_info_p = XRRGetCrtcInfo (display_2,
                                    screen_resources_p,
                                    output_info_p->crtc);
      ACE_ASSERT (crtc_info_p);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("found output \"%s\" in X11 session \"%s\" [%dx%d+%dx%d]\n"),
                  ACE_TEXT (outputName_in.c_str ()),
                  (display_p ? ACE_TEXT (display_p) : ACE_TEXT ("NULL")),
                  crtc_info_p->x, crtc_info_p->y, crtc_info_p->width, crtc_info_p->height));
      XRRFreeCrtcInfo (crtc_info_p); crtc_info_p = NULL;
      XRRFreeOutputInfo (output_info_p); output_info_p = NULL;
    } // end FOR
    XRRFreeScreenResources (screen_resources_p); screen_resources_p = NULL;
  } // end FOR

  result = XCloseDisplay (display_2); display_2 = NULL;
  if (result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XCloseDisplay(), continuing\n")));

  return (display_p ? ACE_TEXT_ALWAYS_CHAR (display_p) : ACE_TEXT_ALWAYS_CHAR (""));
}

XWindowAttributes
Common_UI_X11_Tools::get (const struct _XDisplay& display_in,
                          Window id_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_X11_Tools::get"));

  // initialize return value(s)
  XWindowAttributes return_value;
  ACE_OS::memset (&return_value, 0, sizeof (XWindowAttributes));

  Status result = XGetWindowAttributes (&const_cast<Display&> (display_in),
                                        id_in,
                                        &return_value);
  ACE_ASSERT (result == True); ACE_UNUSED_ARG (result);

  return return_value;
}

void
Common_UI_X11_Tools::dump (const struct _XDisplay& display_in,
                           Drawable id_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_X11_Tools::dump"));

  Window window_i = 0;
  int relative_position_x = 0, relative_position_y = 0;
  Common_UI_Resolution_t resolution_s;
  unsigned int border_width_i = 0, depth_i = 0;
  Status result = XGetGeometry (&const_cast<Display&> (display_in),
                                id_in,
                                &window_i,
                                &relative_position_x, &relative_position_y,
                                &resolution_s.width, &resolution_s.height,
                                &border_width_i,
                                &depth_i);
  ACE_ASSERT (result == True); ACE_UNUSED_ARG (result);
  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("drawable %u:\n\twindow: %u\n\tx/y: [%u,%u]; size: [%ux%u]\n\tborder: %u; depth: %u\n"),
              id_in,
              window_i,
              relative_position_x, relative_position_y, resolution_s.width, resolution_s.height,
              border_width_i, depth_i));
}

std::string
Common_UI_X11_Tools::toString (const struct _XDisplay& display_in,
                               int errorCode_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_X11_Tools::toString"));

  // initialize return value(s)
  std::string return_value;

  char buffer_a[BUFSIZ];
  ACE_OS::memset (&buffer_a, 0, sizeof (char[BUFSIZ]));

  Status result = XGetErrorText (&const_cast<Display&> (display_in),
                                 errorCode_in,
                                 buffer_a, sizeof (char[BUFSIZ]));
  if (unlikely (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XGetErrorText(0x%@,%d): \"%m\", aborting\n"),
                &display_in,
                errorCode_in));
    return return_value;
  } // end IF
  return_value = buffer_a;

  return return_value;
}

Common_UI_Resolution_t
Common_UI_X11_Tools::toResolution (const struct _XDisplay& display_in,
                                   Window window_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_X11_Tools::toResolution"));

  Common_UI_Resolution_t return_value;

  XWindowAttributes attributes_s;
  ACE_OS::memset (&attributes_s, 0, sizeof (XWindowAttributes));
  Status result = XGetWindowAttributes (&const_cast<Display&> (display_in),
                                        window_in,
                                        &attributes_s);
  if (unlikely (!result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to XGetWindowAttributes(0x%@,%u): \"%s\", aborting\n"),
                &display_in, window_in,
                ACE_TEXT (Common_UI_X11_Tools::toString (display_in, result).c_str ())));
    return return_value;
  } // end IF
  return_value.width = attributes_s.width;
  return_value.height = attributes_s.height;

  return return_value;
}
