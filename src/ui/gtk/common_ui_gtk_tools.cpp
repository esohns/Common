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

#include "common_ui_gtk_tools.h"

#include <limits>
#include <sstream>
#include <string>

#if GTK_CHECK_VERSION (2,3,0)
#include "glib-object.h"
#endif // GTK_CHECK_VERSION (2,3,0)

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
// #include "gdk/gdkwayland.h"
#include "gdk/gdkdisplay.h"
#include "gdk/gdkx.h"
#endif // ACE_WIN32 || ACE_WIN64

#if defined (GTK2_USE)
#if defined (LIBGLADE_SUPPORT)
#include "glade/glade-init.h"
#endif // LIBGLADE_SUPPORT
#endif // GTK2_USE

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

// initialize statics
bool Common_UI_GTK_Tools::GTKInitialized = false;

#if defined (_DEBUG)
#if GTK_CHECK_VERSION (4,0,0)
#else
void
gtk_container_dump_cb (GtkWidget* widget_in,
                       gpointer userData_in)
{
  COMMON_TRACE (ACE_TEXT ("::gtk_container_dump_cb"));

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (userData_in);

  unsigned int* indent_p = reinterpret_cast<unsigned int*> (userData_in);

  GList* list_p = gtk_container_get_children (GTK_CONTAINER (widget_in));
  ACE_ASSERT (list_p);

  std::string indent_string (*indent_p, '\t');
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s%s [%u children]:\n"),
              ACE_TEXT (indent_string.c_str ()),
              ACE_TEXT (gtk_widget_get_name (widget_in)),
              g_list_length (list_p)));

  unsigned int indent_i = *indent_p + 1;
  gtk_container_foreach (GTK_CONTAINER (widget_in),
                         gtk_container_dump_cb,
                         &indent_i);
}
#endif // GTK_CHECK_VERSION (4,0,0)
#endif // _DEBUG

gboolean
gtk_tree_model_foreach_find_index_cb (GtkTreeModel* treeModel_in,
                                      GtkTreePath* treePath_in,
                                      GtkTreeIter* treeIterator_in,
                                      gpointer userData_in)
{
  COMMON_TRACE (ACE_TEXT ("::gtk_tree_model_foreach_find_index_cb"));

  ACE_UNUSED_ARG (treeModel_in);
  ACE_UNUSED_ARG (treePath_in);

  // sanity check(s)
  ACE_ASSERT (treeModel_in);
  ACE_ASSERT (treeIterator_in);
  ACE_ASSERT (userData_in);
  struct common_ui_gtk_tools_treemodel_indexsearch_cbdata* cb_data_p =
      reinterpret_cast<struct common_ui_gtk_tools_treemodel_indexsearch_cbdata*> (userData_in);
  ACE_ASSERT (cb_data_p);

#if GTK_CHECK_VERSION (2,30,0)
  GValue value = G_VALUE_INIT;
#else
  GValue value;
  ACE_OS::memset (&value, 0, sizeof (struct _GValue));
#endif // GTK_CHECK_VERSION (2,30,0)
  gtk_tree_model_get_value (treeModel_in,
                            treeIterator_in,
                            cb_data_p->column, &value);
  ACE_ASSERT (G_VALUE_HOLDS (&value, G_VALUE_TYPE (&cb_data_p->value)));
  switch (G_VALUE_TYPE (&cb_data_p->value))
  {
    case G_TYPE_CHAR:
    {
#if GTK_CHECK_VERSION (3,0,0)
      cb_data_p->found =
          (g_value_get_schar (&cb_data_p->value) == g_value_get_schar (&value));
#else
      cb_data_p->found =
        (g_value_get_schar (&cb_data_p->value) == g_value_get_schar (&value));
#endif // GTK_CHECK_VERSION (3,0,0)
      break;
    }
    case G_TYPE_UCHAR:
    {
      cb_data_p->found =
          (g_value_get_uchar (&cb_data_p->value) == g_value_get_uchar (&value));
      break;
    }
    case G_TYPE_BOOLEAN:
    {
      cb_data_p->found =
          (g_value_get_boolean (&cb_data_p->value) == g_value_get_boolean (&value));
      break;
    }
    case G_TYPE_INT:
    {
      cb_data_p->found =
          (g_value_get_int (&cb_data_p->value) == g_value_get_int (&value));
      break;
    }
    case G_TYPE_UINT:
    {
      cb_data_p->found =
          (g_value_get_uint (&cb_data_p->value) == g_value_get_uint (&value));
      break;
    }
    case G_TYPE_LONG:
    {
      cb_data_p->found =
          (g_value_get_long (&cb_data_p->value) == g_value_get_long (&value));
      break;
    }
    case G_TYPE_ULONG:
    {
      cb_data_p->found =
          (g_value_get_ulong (&cb_data_p->value) == g_value_get_ulong (&value));
      break;
    }
    case G_TYPE_INT64:
    {
      cb_data_p->found =
          (g_value_get_int64 (&cb_data_p->value) == g_value_get_int64 (&value));
      break;
    }
    case G_TYPE_UINT64:
    {
      cb_data_p->found =
          (g_value_get_uint64 (&cb_data_p->value) == g_value_get_uint64 (&value));
      break;
    }
    case G_TYPE_FLOAT:
    {
      cb_data_p->found =
          (g_value_get_float (&cb_data_p->value) == g_value_get_float (&value));
      break;
    }
    case G_TYPE_DOUBLE:
    {
      cb_data_p->found =
          (g_value_get_double (&cb_data_p->value) == g_value_get_double (&value));
      break;
    }
    case G_TYPE_STRING:
    {
      cb_data_p->found =
        !ACE_OS::strcmp (g_value_get_string (&cb_data_p->value),
                         g_value_get_string (&value));
      break;
    }
    case G_TYPE_POINTER:
    {
      cb_data_p->found =
          (g_value_get_pointer (&cb_data_p->value) == g_value_get_pointer (&value));
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown value type (was: %d: %s), aborting\n"),
                  G_VALUE_TYPE (&cb_data_p->value), ACE_TEXT (G_VALUE_TYPE_NAME (&cb_data_p->value))));
      return TRUE;
    }
  } // end SWITCH
  g_value_unset (&value);
  if (cb_data_p->found)
    return TRUE;

  cb_data_p->index++;
  return FALSE;
}

// ---------------------------------------

bool
Common_UI_GTK_Tools::initialize (int argc_in,
                                 ACE_TCHAR* argv_in[])
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::initialize"));

  // sanity check(s)
  if (Common_UI_GTK_Tools::GTKInitialized)
    return true;

  gtk_disable_setlocale ();

#if GTK_CHECK_VERSION (4,0,0)
#undef gtk_init
  gtk_init ();
#else
#if defined (_DEBUG)
  if (!gtk_init_check (&argc_in, &argv_in))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to gtk_init_check(): \"%m\", aborting\n")));
    return false;
  } // end IF
#else
  gtk_init (&argc_in, &argv_in);
#endif // _DEBUG
#endif // GTK_CHECK_VERSION (4,0,0)
//  GOptionEntry entries_a[] = { {NULL} };
//  if (unlikely (!gtk_init_with_args (&argc_,     // argc
//                                     &argv_,     // argv
//                                     NULL,       // parameter string
//                                     entries_a,  // entries
//                                     NULL,       // translation domain
//                                     &error_p))) // error
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to gtk_init_with_args(): \"%s\", aborting\n"),
//                ACE_TEXT (error_p->message)));
//    g_error_free (error_p); error_p = NULL;
//    goto error;
//  } // end IF

#if GTK_CHECK_VERSION (3,0,0)
#else
  /* gdk_rgb_init() is a function which I can only guess sets up the
   * true colour colour map. It returns void so we can't check its
   * return value.
   */
  gdk_rgb_init ();
#endif // GTK_CHECK_VERSION(3,0,0)

#if defined (GTK2_USE)
#if defined (LIBGLADE_SUPPORT)
  // step2: initialize (lib)glade
  glade_init ();
#endif // LIBGLADE_SUPPORT
#endif // GTK2_USE

  Common_UI_GTK_Tools::GTKInitialized = true;

  return true;
}
bool
Common_UI_GTK_Tools::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::finalize"));

  return true;
}

std::string
Common_UI_GTK_Tools::UTF8ToLocale (const gchar* string_in,
                                   gssize length_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::UTF8ToLocale"));

  // initialize result
  std::string result;

  GError* error_p = NULL;
  gchar* string_p = g_locale_from_utf8 (string_in, // text
                                        length_in, // number of bytes
                                        NULL,      // bytes read (don't care)
                                        NULL,      // bytes written (don't care)
                                        &error_p); // return value: error
  if (unlikely (error_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_locale_from_utf8(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (string_in),
                ACE_TEXT (error_p->message)));
    g_error_free (error_p); error_p = NULL;
  } // end IF
  else
    result = string_p;

  // clean up
  g_free (string_p); string_p = NULL;

  return result;
}

gchar*
Common_UI_GTK_Tools::localeToUTF8 (const std::string& string_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::localeToUTF8"));

  // initialize return value(s)
  gchar* result_p = NULL;

  GError* error_p = NULL;
  result_p = g_locale_to_utf8 (string_in.c_str (), // text
                               -1,                 // \0-terminated
                               NULL,               // bytes read (don't care)
                               NULL,               // bytes written (don't care)
                               &error_p);          // return value: error
  if (unlikely (error_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_locale_to_utf8(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (string_in.c_str ()),
                ACE_TEXT (error_p->message)));
    g_error_free (error_p); error_p = NULL;
    return NULL;
  } // end IF

  return result_p;
}

gint
Common_UI_GTK_Tools::valueToIndex (GtkTreeModel* treeModel_in,
                                   const GValue& value_in,
                                   gint column_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::valueToIndex"));

  // sanity check(s)
  ACE_ASSERT (treeModel_in);
  if (!gtk_tree_model_iter_n_children (treeModel_in,
                                       NULL))
    return -1;

  struct common_ui_gtk_tools_treemodel_indexsearch_cbdata cb_data_s;
  cb_data_s.column = column_in;
  cb_data_s.index = 0;
  g_value_init (&cb_data_s.value, G_VALUE_TYPE (&value_in));
  g_value_copy (&value_in, &cb_data_s.value);
  gtk_tree_model_foreach (treeModel_in,
                          gtk_tree_model_foreach_find_index_cb,
                          &cb_data_s);

  // clean up
  g_value_unset (&cb_data_s.value);

  return (cb_data_s.found ? cb_data_s.index : -1);
}

void
Common_UI_GTK_Tools::selectValue (GtkComboBox* comboBox_in,
                                  const GValue& value_in,
                                  gint column_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::selectValue"));

  gint index_i =
      Common_UI_GTK_Tools::valueToIndex (gtk_combo_box_get_model (comboBox_in),
                                         value_in,
                                         column_in);
  if (index_i != -1)
    gtk_combo_box_set_active (comboBox_in, index_i);
  else
  {
    gchar* string_p = g_strdup_value_contents (&value_in);
    ACE_ASSERT (string_p);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid/unknown identifier (was: \"%s\"), returning\n"),
                string_p));
    g_free (string_p); string_p = NULL;
  } // end ELSE
}

Common_UI_DisplayDevices_t
Common_UI_GTK_Tools::getDisplayDevices ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::getDisplayDevices"));

  // initialize return value(s)
  Common_UI_DisplayDevices_t return_value;

  struct Common_UI_DisplayDevice device_s;
  GdkDisplayManager* display_manager_p = gdk_display_manager_get ();
  ACE_ASSERT (display_manager_p);
  GSList* list_p = gdk_display_manager_list_displays (display_manager_p);
  ACE_ASSERT (list_p);

  GdkDisplay* display_p = NULL;
  int number_of_monitors = 0;
#if GTK_CHECK_VERSION (3,22,0)
  GdkMonitor* monitor_p = NULL;
#else
  int number_of_screens = 0;
  GdkScreen* screen_p = NULL;
#endif // GTK_CHECK_VERSION (3,22,0)
  for (GSList* list_2 = list_p;
       list_2;
       list_2 = list_2->next)
  {
    display_p = (GdkDisplay*)list_2->data;
    ACE_ASSERT (display_p);

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("found display: \"%s\"...\n"),
                ACE_TEXT (gdk_display_get_name (display_p))));

#if GTK_CHECK_VERSION (4,0,0)
    GListModel* monitors_p = gdk_display_get_monitors (display_p);
    for (guint i = 0;
         i < g_list_model_get_n_items (monitors_p);
         ++i)
    {
      monitor_p = GDK_MONITOR (g_list_model_get_object (monitors_p,
                                                        i));
      ACE_ASSERT (monitor_p);

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("found monitor: \"%s\"...\n"),
                  ACE_TEXT (gdk_monitor_get_model (monitor_p))));

      device_s.description += gdk_monitor_get_manufacturer (monitor_p);
      device_s.description += ACE_TEXT_ALWAYS_CHAR (" / ");
      device_s.description += gdk_monitor_get_model (monitor_p);
      //device_s.primary = gdk_monitor_is_primary (monitor_p);
      return_value.push_back (device_s);
    } // end FOR
#elif GTK_CHECK_VERSION (3,22,0)
    number_of_monitors = gdk_display_get_n_monitors (display_p);
    for (int i = 0;
         i < number_of_monitors;
         ++i)
    {
      monitor_p = gdk_display_get_monitor (display_p,
                                           i);
      ACE_ASSERT (monitor_p);

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("found monitor: \"%s\"...\n"),
                  ACE_TEXT (gdk_monitor_get_model (monitor_p))));

      device_s.description += gdk_monitor_get_manufacturer (monitor_p);
      device_s.description += ACE_TEXT_ALWAYS_CHAR (" / ");
      device_s.description += gdk_monitor_get_model (monitor_p);
      device_s.primary = gdk_monitor_is_primary (monitor_p);
      return_value.push_back (device_s);
    } // end FOR
#else
    number_of_screens = gdk_display_get_n_screens (display_p);
    for (int i = 0;
         i < number_of_screens;
         ++i)
    {
      screen_p = gdk_display_get_screen (display_p,
                                         i);
      ACE_ASSERT (screen_p);

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("found screen: \"%s\"...\n"),
                  ACE_TEXT (gdk_screen_get_monitor_plug_name (screen_p,
                                                              i))));

      number_of_monitors = gdk_screen_get_n_monitors (screen_p);
      for (int j = 0;
           j < number_of_monitors;
           ++j)
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("found monitor: \"%s\"...\n"),
                    ACE_TEXT (gdk_screen_get_monitor_plug_name (screen_p,
                                                                i))));

        if (gdk_screen_get_primary_monitor (screen_p) == j)
          device_s.description = ACE_TEXT_ALWAYS_CHAR ("*");
        device_s.description += gdk_screen_get_monitor_plug_name (screen_p,
                                                                  i);
        return_value.push_back (device_s);
      } // end FOR
    } // end FOR
#endif // GTK_CHECK_VERSION (3,22,0)
  } // end FOR
  g_slist_free (list_p); list_p = NULL;

  return return_value;
}

#if defined(ACE_WIN32) || defined(ACE_WIN64)
#else
GdkWindow*
Common_UI_GTK_Tools::get (unsigned long windowId_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::get"));

  GdkWindow* result_p = NULL;

  std::vector<GdkDisplay*> displays_a;
  GdkDisplayManager* display_manager_p = gdk_display_manager_get ();
  ACE_ASSERT (display_manager_p);
  GdkDisplay* display_p = NULL;
  // display_p = gdk_display_manager_get_default_display (display_manager_p);
  // if (!display_p)
  //  display_p = gdk_display_get_default ();
  // if (display_p)
  //   displays_a.push_back (display_p);
  // else
  // {
    GSList* list_p =
     gdk_display_manager_list_displays (display_manager_p);
    for (GSList* iterator = list_p; iterator; iterator = iterator->next)
    {
      display_p = (GdkDisplay*)iterator->data;
      // if (GDK_IS_X11_DISPLAY (display_p))
        displays_a.push_back (display_p);
    } // end FOR
    g_slist_free (list_p); list_p = NULL;
  // } // end ELSE
  // Display* display_2 = XOpenDisplay (NULL);
  // ACE_ASSERT (display_2);
  // display_p = gdk_x11_lookup_xdisplay (display_2);
  // ACE_ASSERT (display_p);
  // XCloseDisplay (display_2); display_2 = NULL;

  for (std::vector<GdkDisplay*>::iterator iterator = displays_a.begin ();
       iterator != displays_a.end ();
       ++iterator)
  {
    result_p = gdk_x11_window_lookup_for_display (*iterator,
                                                  windowId_in);
    if (result_p)
    {
      g_object_ref (result_p); // *NOTE*: consistent with code below (caller
                               //         holds a reference)
      return result_p;
    } // end IF
  } // end FOR

  if (displays_a.empty ())
    return NULL;
  result_p =
    gdk_x11_window_foreign_new_for_display (*displays_a.begin (),
                                            windowId_in);
  return result_p;
}
#endif // ACE_WIN32 || ACE_WIN64

#if GTK_CHECK_VERSION (4,0,0)
GdkPixbuf*
Common_UI_GTK_Tools::get (GdkSurface* surface_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::get"));

  GdkPixbuf* pixel_buffer_p = NULL;

  gint width, height;
  width = gdk_surface_get_width (surface_in);
  height = gdk_surface_get_height (surface_in);
  pixel_buffer_p = gdk_pixbuf_get_from_surface (surface_in,
                                                0, 0,
                                                width, height);

  return pixel_buffer_p;
#else
GdkPixbuf*
Common_UI_GTK_Tools::get (GdkWindow* window_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::get"));

  GdkPixbuf* pixel_buffer_p = NULL;

#if GTK_CHECK_VERSION (3,0,0)
  gint x, y, width, height;
  gdk_window_get_geometry (window_in, &x, &y, &width, &height);
  pixel_buffer_p = gdk_pixbuf_get_from_window (window_in,
                                               x, y,
                                               width, height);
#elif GTK_CHECK_VERSION (2,0,0)
  gint x_orig, y_orig;
  gint width, height;
  gdk_drawable_get_size (window_in, &width, &height);
  gdk_window_get_origin (window_in, &x_orig, &y_orig);

  pixel_buffer_p = gdk_pixbuf_get_from_drawable (NULL,
                                                 window_in,
                                                 NULL,
                                                 x_orig, y_orig,
                                                 0, 0,
                                                 width, height);
#endif // GTK_CHECK_VERSION

  return pixel_buffer_p;
}
#endif // GTK_CHECK_VERSION (4,0,0)

#if defined (_DEBUG)
#if GTK_CHECK_VERSION (4,0,0)
#else
void
Common_UI_GTK_Tools::dump (GtkContainer* container_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::dump"));

  // sanity check(s)
  ACE_ASSERT (container_in);

  GList* list_p = gtk_container_get_children (container_in);
  ACE_ASSERT (list_p);

  unsigned int indent_i = 0;
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s [%u children]:\n"),
              ACE_TEXT (gtk_widget_get_name (GTK_WIDGET (container_in))),
              g_list_length (list_p)));

  gtk_container_foreach (container_in,
                         gtk_container_dump_cb,
                         &indent_i);
}
#endif // GTK_CHECK_VERSION (4,0,0)

void
Common_UI_GTK_Tools::dumpGtkLibraryInfo ()
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::dumpGtkLibraryInfo"));

  std::ostringstream converter;
  std::string information_string =
    ACE_TEXT_ALWAYS_CHAR ("gtk library version: ");
  converter << GTK_MAJOR_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (".");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GTK_MINOR_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (".");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GTK_MICRO_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (" [age (binary/interface): ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GTK_BINARY_AGE;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (", ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GTK_INTERFACE_AGE;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR ("]");

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#else
//  information_string +=
//      ACE_TEXT_ALWAYS_CHAR ("\ngtk OpenGL extension library version: ");
//  converter.clear ();
//  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
//  converter << GDKGLEXT_MAJOR_VERSION;
//  information_string += converter.str ();
//  information_string += ACE_TEXT_ALWAYS_CHAR (".");
//  converter.clear ();
//  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
//  converter << GDKGLEXT_MINOR_VERSION;
//  information_string += converter.str ();
//  information_string += ACE_TEXT_ALWAYS_CHAR (".");
//  converter.clear ();
//  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
//  converter << GDKGLEXT_MICRO_VERSION;
//  information_string += converter.str ();
//  information_string += ACE_TEXT_ALWAYS_CHAR (" [age (binary/interface): ");
//  converter.clear ();
//  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
//  converter << GDKGLEXT_BINARY_AGE;
//  information_string += converter.str ();
//  information_string += ACE_TEXT_ALWAYS_CHAR (", ");
//  converter.clear ();
//  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
//  converter << GDKGLEXT_INTERFACE_AGE;
//  information_string += converter.str ();
//  information_string += ACE_TEXT_ALWAYS_CHAR ("]");
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,0,0)
#endif // GTKGL_SUPPORT

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s\n"),
              ACE_TEXT (information_string.c_str ())));
}

#if defined (GTKGL_SUPPORT)
void
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
Common_UI_GTK_Tools::dumpGtkOpenGLInfo (GdkGLContext* context_in)
#else
#if defined (GTKGLAREA_SUPPORT)
Common_UI_GTK_Tools::dumpGtkOpenGLInfo (GglaContext* context_in)
#else
Common_UI_GTK_Tools::dumpGtkOpenGLInfo (GdkWindow* window_in)
#endif /* GTKGLAREA_SUPPORT */
#endif // GTK_CHECK_VERSION (3,16,0)
#elif GTK_CHECK_VERSION (2,0,0)
#if defined (GTKGLAREA_SUPPORT)
Common_UI_GTK_Tools::dumpGtkOpenGLInfo (GdkGLContext* context_in)
#else
Common_UI_GTK_Tools::dumpGtkOpenGLInfo ()
#endif /* GTKGLAREA_SUPPORT */
#endif // GTK_CHECK_VERSION (3,0,0)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::dumpGtkOpenGLInfo"));

#if GTK_CHECK_VERSION (4,0,0)
  GdkSurface* window_p = NULL;
#elif GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  GdkWindow* window_p = NULL;
  bool release_context = false;
#else
#if defined (GTKGLAREA_SUPPORT)
#else
  bool release_window = false;
#endif /* GTKGLAREA_SUPPORT */
#endif // GTK_CHECK_VERSION (3,16,0)
#endif // GTK_CHECK_VERSION (3/4,0,0)
  std::ostringstream converter;
  std::string information_string;

#if GTK_CHECK_VERSION (4,0,0)
  window_p = gdk_gl_context_get_surface (context_in);
#elif GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  window_p = gdk_gl_context_get_window (context_in);
#else
#if defined (GTKGLAREA_SUPPORT)
#else
#endif /* GTKGLAREA_SUPPORT */
#endif // GTK_CHECK_VERSION (3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#else
#endif /* GTKGLAREA_SUPPORT */
#endif // GTK_CHECK_VERSION

  // sanity check(s)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  ACE_ASSERT (context_in);
#else
#if defined (GTKGLAREA_SUPPORT)
  ACE_ASSERT (context_in);
#else
  window_p = window_in;
  if (unlikely (!window_p))
  {
    GdkWindowAttr window_attributes_s;
    ACE_OS::memset (&window_attributes_s, 0, sizeof (GdkWindowAttr));
    window_attributes_s.width = 100;
    window_attributes_s.height = 100;
    window_attributes_s.wclass = GDK_INPUT_OUTPUT;
    window_attributes_s.window_type = GDK_WINDOW_TOPLEVEL;
    gint attribute_mask = 0;
    window_p = gdk_window_new (NULL,
                               &window_attributes_s,
                               attribute_mask);
    if (!window_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gdk_window_new(), returning\n")));
      return;
    } // end IF
    release_window = true;
  } // end IF
  ACE_ASSERT (window_p);
#endif /* GTKGLAREA_SUPPORT */
#endif // GTK_CHECK_VERSION (3,16,0)
#elif GTK_CHECK_VERSION (2,0,0)
#if defined (GTKGLAREA_SUPPORT)
  ACE_ASSERT (context_in);
#endif // GTKGLAREA_SUPPORT
#else
  ACE_ASSERT (false); // *TODO*: use glext
#endif // GTK_CHECK_VERSION (3,0,0)
//  ACE_ASSERT (window_p);

#if GTK_CHECK_VERSION (4,0,0)
  GdkGLContext* gl_context_p = context_in;
#elif GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  GdkGLContext* gl_context_p = gdk_gl_context_get_current ();
  if (unlikely (!gl_context_p))
  {
    GError* error_p = NULL;
    // *TODO*: this currently fails on Wayland (Gnome 3.22.24)
    // *WORKAROUND*: set GDK_BACKEND=x11 environment to force XWayland
    gl_context_p = gdk_window_create_gl_context (window_p,
                                                 &error_p);
    if (unlikely (!gl_context_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gdk_window_create_gl_context(0x%@): \"%s\", returning\n"),
                  window_p,
                  ACE_TEXT (error_p->message)));
      g_error_free (error_p); error_p = NULL;
      return;
    } // end IF
    release_context = true;
#if defined (_DEBUG)
    gdk_gl_context_set_debug_enabled (gl_context_p, TRUE);
#endif // _DEBUG
    if (!gdk_gl_context_realize (gl_context_p,
                                 &error_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gdk_gl_context_realize(0x%@): \"%s\", returning\n"),
                  gl_context_p,
                  ACE_TEXT (error_p->message)));
      g_error_free (error_p); error_p = NULL;
      g_object_unref (gl_context_p); gl_context_p = NULL;
      return;
    } // end IF
  } // end IF
  ACE_ASSERT (gl_context_p);
#else
#if defined (GTKGLAREA_SUPPORT)
  gint result = ggla_query ();
  if (!result)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("OpenGL not supported\n")));
    return;
  } // end IF

  gchar* info_string_p = ggla_get_info ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("OpenGL information: \"%s\"\n"),
              ACE_TEXT (info_string_p)));
  g_free (info_string_p); info_string_p = NULL;
#else
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
  gint result = gdk_gl_query ();
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("OpenGL not supported, returning\n")));
    return;
  } // end IF

  gchar* info_string_p = gdk_gl_get_info ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("OpenGL information: \"%s\"\n"),
              ACE_TEXT (info_string_p)));
  g_free (info_string_p); info_string_p = NULL;
#else
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3/4,0,0)

  int version_major = 0, version_minor = 0;

  information_string = ACE_TEXT_ALWAYS_CHAR ("OpenGL version: ");
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  ACE_ASSERT (gl_context_p);

  gdk_gl_context_get_version (gl_context_p,
                              &version_major,
                              &version_minor);
#else
  information_string.clear ();
  goto continue_;
#endif // GTK_CHECK_VERSION(3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
  information_string.clear ();
  goto continue_;
#else
  if (!gdk_gl_query_extension ())
  {
    information_string += ACE_TEXT_ALWAYS_CHAR ("not supported");
    goto continue_;
  } // end IF

  if (!gdk_gl_query_version (&version_major, &version_minor))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_query_version(), returning\n")));
    return;
  } // end IF
#endif /* GTKGLAREA_SUPPORT */
#endif // GTK_CHECK_VERSION (3,0,0)

  converter << version_major;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (".");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << version_minor;
  information_string += converter.str ();

#if GTK_CHECK_VERSION (3,16,0)
#else
continue_:
#endif // GTK_CHECK_VERSION (3,16,0)
  if (!information_string.empty ())
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s\n"),
                ACE_TEXT (information_string.c_str ())));

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  if (release_context)
    g_object_unref (gl_context_p);
#else
#endif // GTK_CHECK_VERSION (3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#else
#endif /* GTKGLAREA_SUPPORT */
#endif // GTK_CHECK_VERSION (3,0,0)
}
#endif // GTKGL_SUPPORT
#endif // _DEBUG

void
Common_UI_GTK_Tools::dump (GtkBuilder* builder_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::dump"));

  GSList* list_p = NULL, *list_2 = NULL;

  list_p = gtk_builder_get_objects (builder_in);
  ACE_ASSERT (list_p);

  g_print ("%p\n", list_p);
  for (list_2 = list_p;
       list_2 != NULL;
       list_2 = list_2->next)
  {
    g_print ("%p\n", (char *)(list_2->data));
  } // end FOR

  g_slist_free (list_p); list_p = NULL;
}
