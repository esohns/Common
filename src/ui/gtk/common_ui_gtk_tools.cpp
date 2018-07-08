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

#include "ace/Synch.h"
#include "common_ui_gtk_tools.h"

#include <limits>
#include <sstream>
#include <string>

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
#include "gtkgl/gdkgl.h"
#endif // GTK_CHECK_VERSION (3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#include "gtkgl/gdkgl.h"
#else
#include "gtk/gtkgl.h" // gtkglext
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,0,0)
#endif // GTKGL_SUPPORT

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_macros.h"

#if defined (_DEBUG)
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
#endif

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

  struct Common_UI_GTK_Tools::TreeModel_IndexSearch_CBData* cb_data_p =
      reinterpret_cast<struct Common_UI_GTK_Tools::TreeModel_IndexSearch_CBData*> (userData_in);
  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  GValue value;
#if GTK_CHECK_VERSION (3,0,0)
  value = G_VALUE_INIT;
//#else
//  g_value_init (&value, G_VALUE_TYPE (&cb_data_s.value));
#endif
  gtk_tree_model_get_value (treeModel_in,
                            treeIterator_in,
                            cb_data_p->column, &value);
  ACE_ASSERT (G_VALUE_HOLDS (&value, G_VALUE_TYPE (&cb_data_p->value)));
  switch (G_VALUE_TYPE (&cb_data_p->value))
  {
    case G_TYPE_CHAR:
    {
      cb_data_p->found =
          (g_value_get_char (&cb_data_p->value) == g_value_get_char (&value));
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

    // clean up
    g_error_free (error_p);
  } // end IF
  else
    result = string_p;

  // clean up
  g_free (string_p);

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

      // clean up
    g_error_free (error_p);

    return NULL;
  } // end IF

  return result_p;
}

guint
Common_UI_GTK_Tools::valueToIndex (GtkTreeModel* treeModel_in,
                                   const GValue& value_in,
                                   gint column_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::valueToIndex"));

  // sanity check(s)
  ACE_ASSERT (treeModel_in);
  if (!gtk_tree_model_iter_n_children (treeModel_in,
                                       NULL))
    return std::numeric_limits<guint>::max ();

  struct Common_UI_GTK_Tools::TreeModel_IndexSearch_CBData cb_data_s;
  cb_data_s.column = column_in;
  cb_data_s.index = 0;
  g_value_init (&cb_data_s.value, G_VALUE_TYPE (&value_in));
  g_value_copy (&value_in, &cb_data_s.value);
  gtk_tree_model_foreach (treeModel_in,
                          gtk_tree_model_foreach_find_index_cb,
                          &cb_data_s);

  // clean up
  g_value_unset (&cb_data_s.value);

  return (cb_data_s.found ? cb_data_s.index
                          : std::numeric_limits<guint>::max ());
}

bool
Common_UI_GTK_Tools::getDisplayDevices (Common_UI_DisplayDevices_t& devices_out)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::getDisplayDevices"));

  // initialize return value(s)
  devices_out.clear ();

  struct Common_UI_DisplayDevice device_s;
  GdkDisplayManager* display_manager_p = gdk_display_manager_get ();
  ACE_ASSERT (display_manager_p);
  GSList* list_p = gdk_display_manager_list_displays (display_manager_p);
  ACE_ASSERT (list_p);

  GdkDisplay* display_p = NULL;
  int number_of_monitors = 0;
  GdkMonitor* monitor_p = NULL;
  for (GSList* list_2 = list_p;
       list_2;
       list_2 = list_2->next)
  {
    display_p = GDK_DISPLAY (list_2->data);
    ACE_ASSERT (display_p);

#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("found display: \"%s\"...\n"),
                ACE_TEXT (gdk_display_get_name (display_p))));
#endif // _DEBUG

    number_of_monitors = gdk_display_get_n_monitors (display_p);
    for (int i = 0;
         i < number_of_monitors;
         ++i)
    {
      monitor_p = gdk_display_get_monitor (display_p,
                                           i);
      ACE_ASSERT (monitor_p);

#if defined (_DEBUG)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("found monitor: \"%s\"...\n"),
                  ACE_TEXT (gdk_monitor_get_model (monitor_p))));
#endif // _DEBUG

      if (gdk_monitor_is_primary (monitor_p))
        device_s.description = ACE_TEXT_ALWAYS_CHAR ("*");
      device_s.description += gdk_monitor_get_manufacturer (monitor_p);
      device_s.description += ACE_TEXT_ALWAYS_CHAR (" / ");
      device_s.description += gdk_monitor_get_model (monitor_p);
      devices_out.push_back (device_s);
    } // end FOR
  } // end FOR
  g_slist_free (list_p);

  return true;
}

#if defined (_DEBUG)
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
  information_string +=
      ACE_TEXT_ALWAYS_CHAR ("\ngtk OpenGL extension library version: ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GDKGLEXT_MAJOR_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (".");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GDKGLEXT_MINOR_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (".");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GDKGLEXT_MICRO_VERSION;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (" [age (binary/interface): ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GDKGLEXT_BINARY_AGE;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR (", ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << GDKGLEXT_INTERFACE_AGE;
  information_string += converter.str ();
  information_string += ACE_TEXT_ALWAYS_CHAR ("]");
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
Common_UI_GTK_Tools::dumpGtkOpenGLInfo (GdkWindow* window_in)
#else
Common_UI_GTK_Tools::dumpGtkOpenGLInfo (GdkGLContext* context_in)
#endif // GTK_CHECK_VERSION (3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
Common_UI_GTK_Tools::dumpGtkOpenGLInfo ()
#else
Common_UI_GTK_Tools::dumpGtkOpenGLInfo (GdkGLContext* context_in)
#endif /* GTKGLAREA_SUPPORT */
#endif // GTK_CHECK_VERSION (3,0,0)
{
  COMMON_TRACE (ACE_TEXT ("Common_UI_GTK_Tools::dumpGtkOpenGLInfo"));

  GdkWindow* window_p = NULL;
  bool release_window = false;

  // sanity check(s)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
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
#else
  ACE_ASSERT (context_in);
#endif // GTK_CHECK_VERSION (3,16,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#else
  ACE_ASSERT (context_in);
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,0,0)

  std::ostringstream converter;
  std::string information_string;

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  ACE_ASSERT (window_p);

  GdkGLContext* gl_context_p = gdk_gl_context_get_current ();
  bool release_context = false;
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
                  window_in,
                  ACE_TEXT (error_p->message)));

      g_error_free (error_p);
      if (release_window)
        g_object_unref (window_p);

      return;
    } // end IF
    release_context = true;
#if defined (_DEBUG)
    gdk_gl_context_set_debug_enabled (gl_context_p,
                                      TRUE);
#endif
    if (!gdk_gl_context_realize (gl_context_p,
                                 &error_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gdk_gl_context_realize(0x%@): \"%s\", returning\n"),
                  gl_context_p,
                  ACE_TEXT (error_p->message)));

      g_error_free (error_p);
      if (release_window)
        g_object_unref (window_p);
      g_object_unref (gl_context_p);

      return;
    } // end IF
  } // end IF
  ACE_ASSERT (gl_context_p);
#else
#endif // GTK_CHECK_VERSION (3,16,0)
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
#else
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION (3,0,0)

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
#endif // GTK_CHECK_VERSION (3,16,0)
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
  if (release_window)
    g_object_unref (window_p);
#else
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
#else
#endif /* GTKGLAREA_SUPPORT */
#endif // GTK_CHECK_VERSION (3,0,0)
}
#endif // GTKGL_SUPPORT
#endif // _DEBUG
