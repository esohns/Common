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

#include "test_i_gtk_callbacks.h"

#include <sstream>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/GL.h"
#include "gl/GLU.h"

#include "ks.h"
#else
#include "GL/gl.h"
#include "GL/glu.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/config-lite.h"

#include "gdk/gdkkeysyms.h"

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/Synch_Traits.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "common_tools.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "common_file_tools.h"

#include "common_gl_defines.h"
#include "common_gl_texture.h"
#include "common_gl_tools.h"

#include "common_timer_manager.h"

#include "common_ui_defines.h"
#include "common_ui_ifullscreen.h"

#include "common_ui_gtk_common.h"
#include "common_ui_gtk_defines.h"
#include "common_ui_gtk_manager_common.h"
#include "common_ui_gtk_tools.h"

#include "test_i_gtk_defines.h"

void
load_entries (GtkListStore* listStore_in)
{
  gtk_list_store_clear (listStore_in);

  GtkTreeIter iterator;
  std::ostringstream converter;
  for (unsigned int i = 0;
       i < 10;
       ++i)
  {
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << i;

    gtk_list_store_append (listStore_in, &iterator);
    gtk_list_store_set (listStore_in, &iterator,
                        0, converter.str ().c_str (),
                        1, converter.str ().c_str (),
                        -1);
  } // end FOR
}

/////////////////////////////////////////

gboolean
idle_initialize_UI_cb (gpointer userData_in)
{
  // sanity check(s)
  struct Common_UI_GTK_CBData* ui_cb_data_p =
    static_cast<struct Common_UI_GTK_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);
  Common_UI_GTK_BuildersIterator_t iterator =
    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

  // step1: initialize dialog window(s)
  GtkWidget* dialog_p =
    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT (dialog_p);
  //  GtkWidget* image_icon_p = gtk_image_new_from_file (path.c_str ());
  //  ACE_ASSERT (image_icon_p);
  //  gtk_window_set_icon (GTK_WINDOW (dialog_p),
  //                       gtk_image_get_pixbuf (GTK_IMAGE (image_icon_p)));
  //GdkWindow* dialog_window_p = gtk_widget_get_window (dialog_p);
  //gtk_window4096_set_title (,
  //                      caption.c_str ());

//  GtkWidget* about_dialog_p =
//    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
//                                        ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DIALOG_ABOUT_NAME)));
//  ACE_ASSERT (about_dialog_p);

  GtkListStore* list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_LISTSTORE_SOURCE_NAME)));
  ACE_ASSERT (list_store_p);
  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list_store_p),
                                        1, GTK_SORT_DESCENDING);
  load_entries (list_store_p);
  GtkComboBox* combo_box_p =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_COMBOBOX_SOURCE_NAME)));
  ACE_ASSERT (combo_box_p);
  //gtk_combo_box_set_model (combo_box_p,
  //                         GTK_TREE_MODEL (list_store_p));
  GtkCellRenderer* cell_renderer_p = gtk_cell_renderer_text_new ();
  if (!cell_renderer_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to gtk_cell_renderer_text_new(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                              true);
  // *NOTE*: cell_renderer_p does not need to be g_object_unref()ed because it
  //         is GInitiallyUnowned and the floating reference has been
  //         passed to combo_box_p by the gtk_cell_layout_pack_start() call
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                                  //"cell-background", 0,
                                  //"text", 1,
                                  "text", 0,
                                  NULL);

//  combo_box_p =
//      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
//                                             ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_COMBOBOX_SOURCE_2_NAME)));
//  ACE_ASSERT (combo_box_p);
//  //gtk_combo_box_set_model (combo_box_p,
//  //                         GTK_TREE_MODEL (list_store_p));
//  cell_renderer_p = gtk_cell_renderer_text_new ();
//  if (!cell_renderer_p)
//  {
//    ACE_DEBUG ((LM_CRITICAL,
//                ACE_TEXT ("failed to gtk_cell_renderer_text_new(), aborting\n")));
//    return G_SOURCE_REMOVE;
//  } // end IF
//  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
//                              true);
//  // *NOTE*: cell_renderer_p does not need to be g_object_unref()ed because it
//  //         is GInitiallyUnowned and the floating reference has been
//  //         passed to combo_box_p by the gtk_cell_layout_pack_start() call
//  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
//                                  //"cell-background", 0,
//                                  //"text", 1,
//                                  "text", 0,
//                                  NULL);

//  GtkFileChooserButton* file_chooser_button_p =
//    GTK_FILE_CHOOSER_BUTTON (gtk_builder_get_object ((*iterator).second.second,
//                                                     ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_FILECHOOSERBUTTON_SAVE_NAME)));
//  ACE_ASSERT (file_chooser_button_p);
  //GtkFileChooserDialog* file_chooser_dialog_p =
  //  GTK_FILE_CHOOSER_DIALOG (gtk_builder_get_object ((*iterator).second.second,
  //                                                   ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_FILECHOOSERDIALOG_SAVE_NAME)));
  //ACE_ASSERT (file_chooser_dialog_p);

//  toggle_button_p =
//    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
//                                               ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_TOGGLEBUTTON_FULLSCREEN_NAME)));
//  ACE_ASSERT (toggle_button_p);
//  gtk_toggle_button_set_active (toggle_button_p,
//                                is_fullscreen_b);

  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_PROGRESSBAR_NAME)));
  ACE_ASSERT (progress_bar_p);
  gint width, height;
  gtk_widget_get_size_request (GTK_WIDGET (progress_bar_p), &width, &height);
  gtk_progress_bar_set_pulse_step (progress_bar_p,
                                   1.0 / static_cast<double> (width));
  gtk_progress_bar_set_text (progress_bar_p,
                             ACE_TEXT_ALWAYS_CHAR (""));

//  PangoFontDescription* font_description_p =
//    pango_font_description_from_string (ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_PANGO_LOG_FONT_DESCRIPTION));
//  if (!font_description_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to pango_font_description_from_string(\"%s\"): \"%m\", aborting\n"),
//                ACE_TEXT (TEST_I_UI_GTK_PANGO_LOG_FONT_DESCRIPTION)));
//    return G_SOURCE_REMOVE;
//  } // end IF
//  // apply font
//  GtkRcStyle* rc_style_p = gtk_rc_style_new ();
//  if (!rc_style_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to gtk_rc_style_new(): \"%m\", aborting\n")));
//    return G_SOURCE_REMOVE;
//  } // end IF
//  rc_style_p->font_desc = font_description_p;
//  GdkColor base_colour, text_colour;
//  gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_PANGO_LOG_COLOR_BASE),
//                   &base_colour);
//  rc_style_p->base[GTK_STATE_NORMAL] = base_colour;
//  gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_PANGO_LOG_COLOR_TEXT),
//                   &text_colour);
//  rc_style_p->text[GTK_STATE_NORMAL] = text_colour;
//  rc_style_p->color_flags[GTK_STATE_NORMAL] =
//    static_cast<GtkRcFlags> (GTK_RC_BASE |
//                             GTK_RC_TEXT);
//  gtk_widget_modify_style (GTK_WIDGET (view_p),
//                           rc_style_p);
//  //gtk_rc_style_unref (rc_style_p);
//  g_object_unref (rc_style_p);

  //  GtkTextIter iterator;
  //  gtk_text_buffer_get_end_iter (buffer_p,
  //                                &iterator);
  //  gtk_text_buffer_create_mark (buffer_p,
  //                               ACE_TEXT_ALWAYS_CHAR (NET_UI_SCROLLMARK_NAME),
  //                               &iterator,
  //                               TRUE);
  //  g_object_unref (buffer_p);

  GtkDrawingArea* drawing_area_p =
    GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DRAWINGAREA_NAME)));
  ACE_ASSERT (drawing_area_p);

  // step5: initialize updates
//  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, ui_cb_data_p->UIState->lock, G_SOURCE_REMOVE);
//    // schedule asynchronous updates of the log views
//    guint event_source_id = g_timeout_add_seconds (1,
//                                                   idle_update_log_display_cb,
//                                                   userData_in);
//    if (event_source_id > 0)
//      ui_cb_data_p->UIState->eventSourceIds.insert (event_source_id);
//    else
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to g_timeout_add_seconds(): \"%m\", aborting\n")));
//      return G_SOURCE_REMOVE;
//    } // end ELSE
//    // schedule asynchronous updates of the info view
//    event_source_id =
//      g_timeout_add (COMMON_UI_REFRESH_DEFAULT_WIDGET,
//                     idle_update_info_display_cb,
//                     userData_in);
//    if (event_source_id > 0)
//      ui_cb_data_p->UIState->eventSourceIds.insert (event_source_id);
//    else
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
//      return G_SOURCE_REMOVE;
//    } // end ELSE
//  } // end lock scope

  // step2: (auto-)connect signals/slots
#if GTK_CHECK_VERSION(4,0,0)
#else
  gtk_builder_connect_signals ((*iterator).second.second,
                               ui_cb_data_p);
#endif // GTK_CHECK_VERSION(4,0,0)

  // step6a: connect default signals
#if GTK_CHECK_VERSION(4,0,0)
#else
  gulong result_2 =
      g_signal_connect (dialog_p,
                        ACE_TEXT_ALWAYS_CHAR ("destroy"),
                        G_CALLBACK (gtk_widget_destroyed),
                        &dialog_p);
  ACE_ASSERT (result_2);
#endif // GTK_CHECK_VERSION(4,0,0)

//  result_2 = g_signal_connect_swapped (G_OBJECT (about_dialog_p),
//                                       ACE_TEXT_ALWAYS_CHAR ("response"),
//                                       G_CALLBACK (gtk_widget_hide),
//                                       about_dialog_p);
//  ACE_ASSERT (result_2);

  //result_2 =
  //  g_signal_connect (file_chooser_button_p,
  //                    ACE_TEXT_ALWAYS_CHAR ("file-set"),
  //                    G_CALLBACK (filechooserbutton_cb),
  //                    userData_in);
  //ACE_ASSERT (result_2);
  //result_2 =
  //  g_signal_connect (file_chooser_dialog_p,
  //                    ACE_TEXT_ALWAYS_CHAR ("file-activated"),
  //                    G_CALLBACK (filechooserdialog_cb),
  //                    NULL);
  //ACE_ASSERT (result_2);

  // set defaults
  //file_chooser_button_p =
  //  GTK_FILE_CHOOSER_BUTTON (gtk_builder_get_object ((*iterator).second.second,
  //                                                   ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_FILECHOOSERBUTTON_SAVE_NAME)));
//  ACE_ASSERT (file_chooser_button_p);
//  std::string default_folder_uri = ACE_TEXT_ALWAYS_CHAR ("file://");
//  default_folder_uri += filename_string;
//  gboolean result =
//    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (file_chooser_button_p),
//                                         ACE_TEXT_ALWAYS_CHAR (ACE::dirname (filename_string.c_str ())));
//  if (!result)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to gtk_file_chooser_set_current_folder(\"%s\"): \"%m\", aborting\n"),
//                ACE_TEXT (ACE::dirname (filename_string.c_str ()))));
//    return G_SOURCE_REMOVE;
//  } // end IF

  // step9: draw main dialog
#if GTK_CHECK_VERSION(4,0,0)
  gtk_widget_show (dialog_p);
#else
  gtk_widget_show_all (dialog_p);
#endif // GTK_CHECK_VERSION(4,0,0)

  // step10: retrieve canvas coordinates, window handle and pixel buffer
  GtkAllocation allocation;
  ACE_OS::memset (&allocation, 0, sizeof (GtkAllocation));
  gtk_widget_get_allocation (GTK_WIDGET (drawing_area_p),
                             &allocation);
#if GTK_CHECK_VERSION(4,0,0)
  GdkSurface* window_p =
    gtk_native_get_surface (gtk_widget_get_native (GTK_WIDGET (drawing_area_p)));
#else
  GdkWindow* window_p = gtk_widget_get_window (GTK_WIDGET (drawing_area_p));
#endif // GTK_CHECK_VERSION(4,0,0)
  ACE_ASSERT (window_p);

  // step11: select default capture source (if any)
  //         --> populate the options comboboxes
  list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_LISTSTORE_SOURCE_NAME)));
  ACE_ASSERT (list_store_p);
  gint n_rows =
    gtk_tree_model_iter_n_children (GTK_TREE_MODEL (list_store_p), NULL);
  if (n_rows)
  {
//    GtkFrame* frame_p =
//      GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
//                                         ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_FRAME_SOURCE_NAME)));
//    ACE_ASSERT (frame_p);
//    gtk_widget_set_sensitive (GTK_WIDGET (frame_p), true);

    combo_box_p =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_COMBOBOX_SOURCE_NAME)));
    ACE_ASSERT (combo_box_p);
    gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), TRUE);

    gtk_combo_box_set_active (combo_box_p, static_cast<gint> (0));
  } // end IF

  return G_SOURCE_REMOVE;
}

gboolean
idle_finalize_UI_cb (gpointer userData_in)
{
  struct Common_UI_GTK_CBData* ui_cb_data_p =
    static_cast<struct Common_UI_GTK_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);
  ACE_UNUSED_ARG (ui_cb_data_p);

  return G_SOURCE_REMOVE;
}

//gboolean
//idle_update_log_display_cb (gpointer userData_in)
//{
//  STREAM_TRACE (ACE_TEXT ("::idle_update_log_display_cb"));

//  struct Common_UI_GTK_CBData* ui_cb_data_p =
//    static_cast<struct Common_UI_GTK_CBData*> (userData_in);

//  // sanity check(s)
//  ACE_ASSERT (ui_cb_data_p);
//  Common_UI_GTK_BuildersIterator_t iterator =
//    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
//  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

//  GtkTextView* view_p =
//      GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
//                                             ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_TEXTVIEW_NAME)));
//  ACE_ASSERT (view_p);
//  GtkTextBuffer* buffer_p = gtk_text_view_get_buffer (view_p);
//  ACE_ASSERT (buffer_p);

//  GtkTextIter text_iterator;
//  gtk_text_buffer_get_end_iter (buffer_p,
//                                &text_iterator);

//  gchar* converted_text = NULL;
//  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, ui_cb_data_p->UIState->lock, G_SOURCE_REMOVE);
//    if (ui_cb_data_p->UIState->logStack.empty ())
//      return G_SOURCE_CONTINUE;

//    // step1: convert text
//    for (Common_MessageStackConstIterator_t iterator_2 = ui_cb_data_p->UIState->logStack.begin ();
//         iterator_2 != ui_cb_data_p->UIState->logStack.end ();
//         ++iterator_2)
//    {
//      converted_text = Common_UI_GTK_Tools::localeToUTF8 (*iterator_2);
//      if (!converted_text)
//      {
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to Common_UI_GTK_Tools::localeToUTF8(\"%s\"), aborting\n"),
//                    ACE_TEXT ((*iterator_2).c_str ())));
//        return G_SOURCE_REMOVE;
//      } // end IF

//      // step2: display text
//      gtk_text_buffer_insert (buffer_p,
//                              &text_iterator,
//                              converted_text,
//                              -1);

//      g_free (converted_text); converted_text = NULL;
//    } // end FOR
//    ui_cb_data_p->UIState->logStack.clear ();
//  } // end lock scope

//  // step3: scroll the view accordingly
////  // move the iterator to the beginning of line, so it doesn't scroll
////  // in horizontal direction
////  gtk_text_iter_set_line_offset (&text_iterator, 0);

////  // ...and place the mark at iter. The mark will stay there after insertion
////  // because it has "right" gravity
////  GtkTextMark* text_mark_p =
////      gtk_text_buffer_get_mark (buffer_p,
////                                ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_SCROLLMARK_NAME));
//////  gtk_text_buffer_move_mark (buffer_p,
//////                             text_mark_p,
//////                             &text_iterator);

////  // scroll the mark onscreen
////  gtk_text_view_scroll_mark_onscreen (view_p,
////                                      text_mark_p);
//  GtkAdjustment* adjustment_p =
//      GTK_ADJUSTMENT (gtk_builder_get_object ((*iterator).second.second,
//                                              ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_ADJUSTMENT_NAME)));
//  ACE_ASSERT (adjustment_p);
//  gtk_adjustment_set_value (adjustment_p,
//                            gtk_adjustment_get_upper (adjustment_p));

//  return G_SOURCE_CONTINUE;
//}

//gboolean
//idle_update_info_display_cb (gpointer userData_in)
//{
//  STREAM_TRACE (ACE_TEXT ("::idle_update_info_display_cb"));

//  // sanity check(s)
//  ACE_ASSERT (userData_in);
//  struct Common_UI_GTK_CBData* ui_cb_data_p =
//      static_cast<struct Common_UI_GTK_CBData*> (userData_in);
//  Common_UI_GTK_BuildersIterator_t iterator =
//    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
//  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

//  GtkSpinButton* spin_button_p = NULL;
//  bool is_session_message = false;
//  enum Common_UI_EventType* event_p = NULL;
//  int result = -1;
//  enum Common_UI_EventType event_e = COMMON_UI_EVENT_INVALID;
//  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, ui_cb_data_p->UIState->lock, G_SOURCE_REMOVE);
//    for (Common_UI_Events_t::ITERATOR iterator_2 (ui_cb_data_p->UIState->eventStack);
//         iterator_2.next (event_p);
//         iterator_2.advance ())
//    { ACE_ASSERT (event_p);
//      switch (*event_p)
//      {
//        case COMMON_UI_EVENT_STARTED:
//        {
//          spin_button_p =
//            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
//                                                     ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_SPINBUTTON_DATAMESSAGES_NAME)));
//          ACE_ASSERT (spin_button_p);
//          gtk_spin_button_set_value (spin_button_p, 0.0);
//          spin_button_p =
//            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
//                                                     ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_SPINBUTTON_SESSIONMESSAGES_NAME)));
//          ACE_ASSERT (spin_button_p);
//          gtk_spin_button_set_value (spin_button_p, 0.0);
//          is_session_message = true;
//          break;
//        }
//        case COMMON_UI_EVENT_DATA:
//        {
//          spin_button_p =
//            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
//                                                     ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_SPINBUTTON_DATA_NAME)));
//          ACE_ASSERT (spin_button_p);
//          gtk_spin_button_set_value (spin_button_p,
//                                     static_cast<gdouble> (ui_cb_data_p->progressData.statistic.bytes));

//          spin_button_p =
//            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
//                                                     ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_SPINBUTTON_DATAMESSAGES_NAME)));
//          ACE_ASSERT (spin_button_p);
//          break;
//        }
//        case COMMON_UI_EVENT_FINISHED:
//        {
//          spin_button_p =
//            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
//                                                     ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_SPINBUTTON_SESSIONMESSAGES_NAME)));
//          ACE_ASSERT (spin_button_p);
//          is_session_message = true;
//          break;
//        }
//        case COMMON_UI_EVENT_STATISTIC:
//        {
////#if defined (ACE_WIN32) || defined (ACE_WIN64)
////          spin_button_p =
////            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
////                                                     ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_SPINBUTTON_CAPTUREDFRAMES_NAME)));
////          ACE_ASSERT (spin_button_p);
////          gtk_spin_button_set_value (spin_button_p,
////                                     static_cast<gdouble> (ui_cb_data_p->progressData.statistic.capturedFrames));
////#endif
////
////          spin_button_p =
////            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
////                                                     ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_SPINBUTTON_DROPPEDFRAMES_NAME)));
////          ACE_ASSERT (spin_button_p);
////          gtk_spin_button_set_value (spin_button_p,
////                                     static_cast<gdouble> (ui_cb_data_p->progressData.statistic.droppedFrames));

//          spin_button_p =
//            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
//                                                     ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_SPINBUTTON_SESSIONMESSAGES_NAME)));
//          ACE_ASSERT (spin_button_p);

//          is_session_message = true;
//          break;
//        }
//        default:
//        {
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("invalid/unknown event type (was: %d), continuing\n"),
//                      event_e));
//          break;
//        }
//      } // end SWITCH
//      ACE_UNUSED_ARG (is_session_message);
//      gtk_spin_button_spin (spin_button_p,
//                            GTK_SPIN_STEP_FORWARD,
//                            1.0);
//      event_p = NULL;
//    } // end FOR

//    // clean up
//    while (!ui_cb_data_p->UIState->eventStack.is_empty ())
//    {
//      result = ui_cb_data_p->UIState->eventStack.pop (event_e);
//      if (result == -1)
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to ACE_Unbounded_Stack::pop(): \"%m\", continuing\n")));
//    } // end WHILE
//  } // end lock scope

//  return G_SOURCE_CONTINUE;
//}

gboolean
idle_update_progress_cb (gpointer userData_in)
{
  struct Common_UI_GTK_ProgressData* data_p =
      static_cast<struct Common_UI_GTK_ProgressData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->state);

  // synch access
  ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->state->lock, G_SOURCE_REMOVE);

  int result = -1;
  Common_UI_GTK_BuildersIterator_t iterator =
    data_p->state->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->state->builders.end ());

  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_PROGRESSBAR_NAME)));
  ACE_ASSERT (progress_bar_p);

  ACE_THR_FUNC_RETURN exit_status;
  ACE_Thread_Manager* thread_manager_p = ACE_Thread_Manager::instance ();
  ACE_ASSERT (thread_manager_p);
  Common_UI_GTK_PendingActionsIterator_t iterator_2;
  for (Common_UI_GTK_CompletedActionsIterator_t iterator_3 = data_p->completedActions.begin ();
       iterator_3 != data_p->completedActions.end ();
       ++iterator_3)
  {
    iterator_2 = data_p->pendingActions.find (*iterator_3);
    ACE_ASSERT (iterator_2 != data_p->pendingActions.end ());
    ACE_thread_t thread_id = (*iterator_2).second.id ();
    result = thread_manager_p->join (thread_id, &exit_status);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Manager::join(%d): \"%m\", continuing\n"),
                  thread_id));
    else
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("thread %u has joined (status was: %u)\n"),
                  thread_id,
                  exit_status));
#else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("thread %u has joined (status was: 0x%@)\n"),
                  thread_id,
                  exit_status));
#endif
    } // end ELSE

    data_p->state->eventSourceIds.erase (*iterator_3);
    data_p->pendingActions.erase (iterator_2);
  } // end FOR
  data_p->completedActions.clear ();

  bool done = false;
  if (data_p->pendingActions.empty ())
  {
    //if (data_p->cursorType != GDK_LAST_CURSOR)
    //{
    //  GdkCursor* cursor_p = gdk_cursor_new (data_p->cursorType);
    //  if (!cursor_p)
    //  {
    //    ACE_DEBUG ((LM_ERROR,
    //                ACE_TEXT ("failed to gdk_cursor_new(%d): \"%m\", continuing\n"),
    //                data_p->cursorType));
    //    return G_SOURCE_REMOVE;
    //  } // end IF
    //  GtkWindow* window_p =
    //    GTK_WINDOW (gtk_builder_get_object ((*iterator).second.second,
    //                                        ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_WINDOW_MAIN)));
    //  ACE_ASSERT (window_p);
    //  GdkWindow* window_2 = gtk_widget_get_window (GTK_WIDGET (window_p));
    //  ACE_ASSERT (window_2);
    //  gdk_window_set_cursor (window_2, cursor_p);
    //  data_p->cursorType = GDK_LAST_CURSOR;
    //} // end IF

    done = true;
  } // end IF

  // synch access
  std::ostringstream converter;
  converter << data_p->eventSourceId;
  converter << ACE_TEXT_ALWAYS_CHAR (" fps");
  gtk_progress_bar_set_text (progress_bar_p,
                             (done ? ACE_TEXT_ALWAYS_CHAR ("")
                                   : converter.str ().c_str ()));
  gtk_progress_bar_pulse (progress_bar_p);

  // reschedule ?
  return (done ? G_SOURCE_REMOVE : G_SOURCE_CONTINUE);
}

//gboolean
//idle_update_video_display_cb (gpointer userData_in)
//{
//  STREAM_TRACE (ACE_TEXT ("::idle_update_video_display_cb"));

//  // sanity check(s)
//  ACE_ASSERT (userData_in);

//  struct Common_UI_GTK_CBData* ui_cb_data_p =
//    static_cast<struct Common_UI_GTK_CBData*> (userData_in);

//  Common_UI_GTK_BuildersIterator_t iterator =
//    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
//  // sanity check(s)
//  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

//  GtkDrawingArea* drawing_area_p =
//    GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
//                                              ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DRAWINGAREA_NAME)));
//  ACE_ASSERT (drawing_area_p);

//  gdk_window_invalidate_rect (gtk_widget_get_window (GTK_WIDGET (drawing_area_p)),
//                              NULL,
//                              false);

//  return G_SOURCE_REMOVE;
//}

//////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
gint
#if GTK_CHECK_VERSION(4,0,0)
dialog_main_delete_cb (GtkWidget* w,
                       GdkEvent* e,
                       gpointer data)
#else
dialog_main_delete_cb (GtkWidget* w,
                       GdkEventAny* e,
                       gpointer data)
#endif // GTK_CHECK_VERSION(4,0,0)
{
  /* callback for "delete" signal */
  g_print ("dialog_main_delete_cb()n");
  return 0;
}
gint
#if GTK_CHECK_VERSION(4,0,0)
dialog_main_destroy_cb (GtkWidget* w,
                        GdkEvent* e,
                        gpointer data)
#else
dialog_main_destroy_cb (GtkWidget* w,
                        GdkEventAny* e,
                        gpointer data)
#endif // GTK_CHECK_VERSION(4,0,0)
{
  /* callback for "destroy" signal */
  g_print("dialog_main_destroy_cb()n");
  return 0;
}

void
togglebutton_record_toggled_cb (GtkToggleButton* toggleButton_in,
                                gpointer userData_in)
{
  bool is_active_b =
      gtk_toggle_button_get_active (toggleButton_in);

  struct Common_UI_GTK_CBData* ui_cb_data_p =
    static_cast<struct Common_UI_GTK_CBData*> (userData_in);

  Common_UI_GTK_BuildersIterator_t iterator =
    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  ACE_UNUSED_ARG (iterator);

  // sanity check(s)
  ACE_ASSERT (ui_cb_data_p);

#if GTK_CHECK_VERSION(4,0,0)
  gtk_button_set_label (GTK_BUTTON (toggleButton_in),
                        (is_active_b ? ACE_TEXT_ALWAYS_CHAR ("_Stop") : ACE_TEXT_ALWAYS_CHAR ("_Record")));
#else
  gtk_button_set_label (GTK_BUTTON (toggleButton_in),
                        (is_active_b ? GTK_STOCK_MEDIA_STOP : GTK_STOCK_MEDIA_RECORD));
#endif // GTK_CHECK_VERSION(4,0,0)

    // step3: start progress reporting
    //ACE_ASSERT (!data_p->progressData.eventSourceId);
    ui_cb_data_p->progressData.eventSourceId =
      //g_idle_add_full (G_PRIORITY_DEFAULT_IDLE, // _LOW doesn't work (on Win32)
      //                 idle_update_progress_cb,
      //                 &data_p->progressData,
      //                 NULL);
      g_timeout_add (//G_PRIORITY_DEFAULT_IDLE,               // _LOW doesn't work (on Win32)
                     COMMON_UI_REFRESH_DEFAULT_PROGRESS_MS, // ms (?)
                     idle_update_progress_cb,
                     &ui_cb_data_p->progressData);//,
                     //NULL);
    if (!ui_cb_data_p->progressData.eventSourceId)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_timeout_add_full(idle_update_progress_cb): \"%m\", returning\n")));
    ui_cb_data_p->progressData.pendingActions[ui_cb_data_p->progressData.eventSourceId] =
      ACE_Thread_ID (0, 0);
    //    ACE_DEBUG ((LM_DEBUG,
    //                ACE_TEXT ("idle_update_progress_cb: %d\n"),
    //                event_source_id));
    ui_cb_data_p->UIState->eventSourceIds.insert (ui_cb_data_p->progressData.eventSourceId);
  } // end lock scope
} // toggleaction_record_toggled_cb

//void
//toggleaction_fullscreen_toggled_cb (GtkToggleAction* toggleAction_in,
//                                    gpointer userData_in)
//{
//  struct Common_UI_GTK_CBData* ui_cb_data_p =
//    static_cast<struct Common_UI_GTK_CBData*> (userData_in);

//  // sanity check(s)
//  ACE_ASSERT (ui_cb_data_p);

//  bool is_active_b =
//#if GTK_CHECK_VERSION(3,0,0)
//      gtk_toggle_action_get_active (toggleAction_in);
//#elif GTK_CHECK_VERSION(2,0,0)
//      gtk_toggle_button_get_active (toggleButton_in);
//#endif // GTK_CHECK_VERSION

//  Common_UI_GTK_BuildersIterator_t iterator =
//    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
//  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

//  Stream_IStreamControlBase* stream_base_p = NULL;
//  Stream_IStream_t* stream_p = NULL;
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  struct Stream_CamSave_DirectShow_UI_CBData* directshow_cb_data_p = NULL;
//  Stream_CamSave_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_stream_iterator;
//  struct Stream_CamSave_MediaFoundation_UI_CBData* mediafoundation_cb_data_p =
//    NULL;
//  Stream_CamSave_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_stream_iterator;
//  switch (ui_cb_data_p->mediaFramework)
//  {
//    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
//    {
//      directshow_cb_data_p =
//        static_cast<struct Stream_CamSave_DirectShow_UI_CBData*> (ui_cb_data_p);
//      stream_base_p = directshow_cb_data_p->stream;
//      stream_p = directshow_cb_data_p->stream;
//      ACE_ASSERT (directshow_cb_data_p->configuration);
//      directshow_stream_iterator =
//        directshow_cb_data_p->configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
//      ACE_ASSERT (directshow_stream_iterator != directshow_cb_data_p->configuration->streamConfiguration.end ());
//      (*directshow_stream_iterator).second.second.fullScreen = is_active;
//      break;
//    }
//    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
//    {
//      mediafoundation_cb_data_p =
//        static_cast<struct Stream_CamSave_MediaFoundation_UI_CBData*> (ui_cb_data_p);
//      stream_base_p = mediafoundation_cb_data_p->stream;
//      stream_p = mediafoundation_cb_data_p->stream;
//      ACE_ASSERT (mediafoundation_cb_data_p->configuration);
//      mediafoundation_stream_iterator =
//        mediafoundation_cb_data_p->configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
//      ACE_ASSERT (mediafoundation_stream_iterator != mediafoundation_cb_data_p->configuration->streamConfiguration.end ());
//      (*mediafoundation_stream_iterator).second.second.fullScreen = is_active;
//      break;
//    }
//    default:
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
//                  ui_cb_data_p->mediaFramework));
//      return;
//    }
//  } // end SWITCH
//#else
//  struct Stream_CamSave_V4L_UI_CBData* cb_data_p =
//    static_cast<struct Stream_CamSave_V4L_UI_CBData*> (ui_cb_data_p);
//  stream_base_p = cb_data_p->stream;
//  stream_p = cb_data_p->stream;
//  ACE_ASSERT (cb_data_p->configuration);
//  Stream_CamSave_V4L_StreamConfiguration_t::ITERATOR_T iterator_2 =
//    cb_data_p->configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
//  ACE_ASSERT (iterator_2 != cb_data_p->configuration->streamConfiguration.end ());
//  (*iterator_2).second.second.fullScreen = is_active_b;
//#endif
//  ACE_ASSERT (stream_base_p);
//  if (!stream_base_p->isRunning ())
//    return;

//  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());
//  GtkWindow* window_p =
//    GTK_WINDOW (gtk_builder_get_object ((*iterator).second.second,
//                                        ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_WINDOW_FULLSCREEN)));
//  ACE_ASSERT (window_p);

//  if (is_active_b)
//  {
//    gtk_widget_show (GTK_WIDGET (window_p));
////  gtk_window_fullscreen (window_p);
//    gtk_window_maximize (window_p);
//  } // end IF
//  else
//  {
////    gtk_window_minimize (window_p);
////  gtk_window_unfullscreen (window_p);
//    gtk_widget_hide (GTK_WIDGET (window_p));
//  } // end ELSE

//  ACE_ASSERT (stream_p);
//  const Stream_Module_t* module_p = NULL;
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  switch (ui_cb_data_p->mediaFramework)
//  {
//    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
//      module_p =
//        stream_p->find (ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_DIRECTSHOW_DEFAULT_NAME_STRING));
//      break;
//    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
//      module_p =
//        stream_p->find (ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_MEDIAFOUNDATION_DEFAULT_NAME_STRING));
//      break;
//    default:
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("%s: invalid/unkown media framework (was: %d), returning\n"),
//                  ACE_TEXT (stream_p->name ().c_str ()),
//                  ui_cb_data_p->mediaFramework));
//      return;
//    }
//  } // end SWITCH
//#else
//  module_p =
//      stream_p->find (ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_GTK_CAIRO_DEFAULT_NAME_STRING));
//#endif
//  if (!module_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to Stream_IStream::find(\"Display\"), returning\n"),
//                ACE_TEXT (stream_p->name ().c_str ())));
//    return;
//  } // end IF
//  Common_UI_IFullscreen* ifullscreen_p =
//    dynamic_cast<Common_UI_IFullscreen*> (const_cast<Stream_Module_t*> (module_p)->writer ());
//  if (!ifullscreen_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s:Display: failed to dynamic_cast<Common_UI_IFullscreen*>(0x%@), returning\n"),
//                ACE_TEXT (stream_p->name ().c_str ()),
//                const_cast<Stream_Module_t*> (module_p)->writer ()));
//    return;
//  } // end IF
//  try {
//    ifullscreen_p->toggle ();
//  } catch (...) {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("caught exception in Common_UI_IFullscreen::toggle(), returning\n")));
//    return;
//  }
//} // toggleaction_fullscreen_toggled_cb

// -----------------------------------------------------------------------------

#if GTK_CHECK_VERSION(4,0,0)
void
on_dialog_response_cb (GtkDialog* dialog_in,
                       gint responseId_in,
                       gpointer userData_in)
{
  ACE_UNUSED_ARG (userData_in);

  switch (responseId_in)
  {
    case GTK_RESPONSE_ACCEPT:
      break;
    default:
      break;
  } // end SWITCH
  gtk_widget_hide (GTK_WIDGET (dialog_in));
}
#endif // GTK_CHECK_VERSION(4,0,0)
gint
button_about_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  ACE_UNUSED_ARG (widget_in);
  struct Common_UI_GTK_CBData* ui_cb_data_p =
    static_cast<struct Common_UI_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (ui_cb_data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

  // retrieve about dialog handle
  GtkDialog* dialog_p =
    GTK_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DIALOG_ABOUT_NAME)));
  if (!dialog_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_builder_get_object(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (TEST_I_UI_GTK_DIALOG_ABOUT_NAME)));
    return TRUE; // propagate
  } // end IF

  // run dialog
#if GTK_CHECK_VERSION(4,0,0)
  gtk_window_set_modal (GTK_WINDOW (dialog_p), TRUE);
  g_signal_connect (dialog_p, "response", G_CALLBACK (on_dialog_response_cb), userData_in);
#else
  gint result = gtk_dialog_run (dialog_p);
  switch (result)
  {
    case GTK_RESPONSE_ACCEPT:
      break;
    default:
      break;
  } // end SWITCH
  gtk_widget_hide (GTK_WIDGET (dialog_p));
#endif // GTK_CHECK_VERSION(4,0,0)

  return FALSE;
} // button_about_clicked_cb

gint
button_quit_clicked_cb (GtkWidget* widget_in,
                        gpointer userData_in)
{
  ACE_UNUSED_ARG (widget_in);
  struct Common_UI_GTK_CBData* ui_cb_data_p =
    static_cast<struct Common_UI_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (ui_cb_data_p);

#if GTK_CHECK_VERSION(4,0,0)
  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop (false, true);
#else
  gtk_main_quit ();
#endif // GTK_CHECK_VERSION(4,0,0)

  return FALSE;
} // button_quit_clicked_cb

//////////////////////////////////////////

void
combobox_source_changed_cb (GtkWidget* widget_in,
                            gpointer userData_in)
{
  struct Common_UI_GTK_CBData* ui_cb_data_p =
    static_cast<struct Common_UI_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (ui_cb_data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

  GtkTreeIter iterator_3;
  if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (widget_in),
                                      &iterator_3))
    return; // <-- nothing selected

  GtkListStore* list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_LISTSTORE_SOURCE_NAME)));
  ACE_ASSERT (list_store_p);
#if GTK_CHECK_VERSION(2,30,0)
  GValue value = G_VALUE_INIT;
#else
  GValue value;
  ACE_OS::memset (&value, 0, sizeof (struct _GValue));
  g_value_init (&value, G_TYPE_STRING);
#endif // GTK_CHECK_VERSION (2,30,0)
  gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                            &iterator_3,
                            1, &value);
  ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
  std::string device_identifier_string = g_value_get_string (&value);
  g_value_unset (&value);

  gint n_rows = 0;

//#if GTK_CHECK_VERSION(3,0,0)
//  GtkToggleAction* toggle_action_p = NULL;
//#elif GTK_CHECK_VERSION(2,0,0)
////  GtkToggleButton* toggle_button_p = NULL;
//#endif // GTK_CHECK_VERSION
//  bool result = false;

  list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_LISTSTORE_SOURCE_NAME)));
  ACE_ASSERT (list_store_p);

  load_entries (list_store_p);

  n_rows =
    gtk_tree_model_iter_n_children (GTK_TREE_MODEL (list_store_p), NULL);
  if (n_rows)
  {
    GtkComboBox* combo_box_p =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_COMBOBOX_SOURCE_2_NAME)));
    ACE_ASSERT (combo_box_p);
    gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), true);
//    gtk_combo_box_set_active (combo_box_p, 0);
  } // end IF
} // combobox_source_changed_cb

void
combobox_source_2_changed_cb (GtkWidget* widget_in,
                              gpointer userData_in)
{
  struct Common_UI_GTK_CBData* ui_cb_data_p =
    static_cast<struct Common_UI_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (ui_cb_data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

  GtkTreeIter iterator_3;
  if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (widget_in),
                                      &iterator_3))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_combo_box_get_active_iter(), returning\n")));
    return;
  } // end IF
  GtkListStore* list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_LISTSTORE_SOURCE_2_NAME)));
  ACE_ASSERT (list_store_p);
#if GTK_CHECK_VERSION(2,30,0)
  GValue value = G_VALUE_INIT;
#else
  GValue value;
  ACE_OS::memset (&value, 0, sizeof (struct _GValue));
  g_value_init (&value, G_TYPE_STRING);
#endif // GTK_CHECK_VERSION (2,30,0)
  gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                            &iterator_3,
                            1, &value);
  ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
  std::string format_string = g_value_get_string (&value);
  g_value_unset (&value);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct _GUID GUID_s = Common_Tools::StringToGUID (format_string);
  ACE_ASSERT (!InlineIsEqualGUID (GUID_s, GUID_NULL));
#else
  __u32 format_i = 0;
  std::istringstream converter;
  converter.str (format_string);
  converter >> format_i;
#endif // ACE_WIN32 || ACE_WIN64
} // combobox_format_changed_cb

#if GTK_CHECK_VERSION(3,0,0)
gboolean
drawingarea_draw_cb (GtkWidget* widget_in,
                     cairo_t* context_in,
                     gpointer userData_in)
{
  // sanity check(s)
  ACE_ASSERT (context_in);
  struct Common_UI_GTK_CBData* ui_cb_data_p =
      static_cast<struct Common_UI_GTK_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);

  GtkAllocation allocation_s;
  gtk_widget_get_allocation (widget_in,
                            &allocation_s);
  cairo_scale (context_in,
              allocation_s.width, allocation_s.height);

  // step1: clear area
  cairo_set_source_rgb (context_in,
                       0.0, 0.0, 0.0); // opaque black
  cairo_rectangle (context_in,
                  0.0, 0.0,
                  1.0, 1.0);
  cairo_fill (context_in);

  // step2: draw something
  static int draw_shape = 0;
  cairo_set_source_rgb (context_in, 1.0, 1.0, 1.0); // opaque white
  cairo_set_line_width (context_in, 0.1);
  switch (++draw_shape % 5)
  {
    case 0: // rectangle
    {
      cairo_rectangle (context_in,
                      0.25, 0.25,
                      0.5, 0.5);
      cairo_fill (context_in);
      break;
    }
    case 1: // circle
    {
      cairo_arc (context_in,
                0.5, 0.5,
                0.25,
                0.0, 2 * M_PI);
      cairo_fill (context_in);
      break;
    }
    case 2: // text
    {
      cairo_font_extents_t fe;
      cairo_text_extents_t te;
      char letter_a[2];
      ACE_OS::memset (letter_a, 0, sizeof (char[2]));
      letter_a[0] = 'a';
      cairo_set_font_size (context_in, 0.5);
      cairo_select_font_face (context_in, "Georgia",
                             CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
      cairo_font_extents (context_in, &fe);
      cairo_text_extents (context_in, letter_a, &te);
      cairo_move_to (context_in,
                    0.5 - te.x_bearing - te.width / 2,
                    0.5 - fe.descent + fe.height / 2);
      cairo_show_text (context_in, letter_a);
      break;
    }
    case 3: // stroke
    {
      cairo_set_line_width (context_in, 0.1);
      cairo_move_to (context_in, 0.0, 0.0);
      cairo_line_to (context_in, 1.0, 1.0);
      cairo_move_to (context_in, 1.0, 0.0);
      cairo_line_to (context_in, 0.0, 1.0);
      cairo_stroke (context_in);
      break;
    }
    case 4: // fill
    {
      //cairo_set_source_rgb (context_in,
      //                      1.0, 1.0, 1.0); // opaque white
      cairo_rectangle (context_in,
                      0.0, 0.0,
                      1.0, 1.0);
      cairo_fill (context_in);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("invalid shape, aborting\n")));
      return FALSE; // propagate event
    }
  } // end SWITCH

  return TRUE; // do NOT propagate the event
}
#else
gboolean
drawingarea_expose_event_cb (GtkWidget* widget_in,
                             GdkEvent* event_in,
                             gpointer userData_in)
{
  ACE_UNUSED_ARG (event_in);

  // sanity check(s)
  struct Common_UI_GTK_CBData* ui_cb_data_p =
      static_cast<struct Common_UI_GTK_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);

  GtkAllocation allocation_s;
  gtk_widget_get_allocation (widget_in,
                             &allocation_s);
  GdkWindow* window_p = gtk_widget_get_window (widget_in);
  ACE_ASSERT (window_p);
  cairo_t* context_p = gdk_cairo_create (GDK_DRAWABLE (window_p));
  ACE_ASSERT (context_p);

  cairo_scale (context_p,
               allocation_s.width, allocation_s.height);

  // step1: clear area
  cairo_set_source_rgb (context_p,
                        0.0, 0.0, 0.0); // opaque black
  cairo_rectangle (context_p,
                   0.0, 0.0,
                   1.0, 1.0);
  cairo_fill (context_p);

  // step2: draw something
  static int draw_shape = 0;
  cairo_set_source_rgb (context_p, 1.0, 1.0, 1.0); // opaque white
  cairo_set_line_width (context_p, 0.1);
  switch (++draw_shape % 5)
  {
    case 0: // rectangle
    {
      cairo_rectangle (context_p,
                       0.25, 0.25,
                       0.5, 0.5);
      cairo_fill (context_p);
      break;
    }
    case 1: // circle
    {
      cairo_arc (context_p,
                 0.5, 0.5,
                 0.25,
                 0.0, 2 * M_PI);
      cairo_fill (context_p);
      break;
    }
    case 2: // text
    {
      cairo_font_extents_t fe;
      cairo_text_extents_t te;
      char letter_a[2];
      ACE_OS::memset (letter_a, 0, sizeof (char[2]));
      letter_a[0] = 'a';
      cairo_set_font_size (context_p, 0.5);
      cairo_select_font_face (context_p, "Georgia",
                              CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
      cairo_font_extents (context_p, &fe);
      cairo_text_extents (context_p, letter_a, &te);
      cairo_move_to (context_p,
                    0.5 - te.x_bearing - te.width / 2,
                    0.5 - fe.descent + fe.height / 2);
      cairo_show_text (context_p, letter_a);
      break;
    }
    case 3: // stroke
    {
      cairo_set_line_width (context_p, 0.1);
      cairo_move_to (context_p, 0.0, 0.0);
      cairo_line_to (context_p, 1.0, 1.0);
      cairo_move_to (context_p, 1.0, 0.0);
      cairo_line_to (context_p, 0.0, 1.0);
      cairo_stroke (context_p);
      break;
    }
    case 4: // fill
    {
      //cairo_set_source_rgb (context_in,
      //                      1.0, 1.0, 1.0); // opaque white
      cairo_rectangle (context_p,
                       0.0, 0.0,
                       1.0, 1.0);
      cairo_fill (context_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid shape, aborting\n")));
      cairo_destroy (context_p); context_p = NULL;
      return FALSE; // propagate event
    }
  } // end SWITCH

  cairo_destroy (context_p); context_p = NULL;

  return TRUE; // do NOT propagate the event
}
#endif // GTK_CHECK_VERSION(3,0,0)

gboolean
drawingarea_configure_event_cb (GtkWidget* window_in,
                                GdkEvent* event_in,
                                gpointer userData_in)
{
  COMMON_TRACE (ACE_TEXT ("::drawingarea_configure_event_cb"));

  // sanity check(s)
  Common_UI_GTK_CBData* data_p =
    static_cast<Common_UI_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  GtkDrawingArea* drawing_area_p =
    GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_DRAWINGAREA_NAME)));
  ACE_ASSERT (drawing_area_p);
  GtkAllocation allocation;
  ACE_OS::memset (&allocation, 0, sizeof (GtkAllocation));
  gtk_widget_get_allocation (GTK_WIDGET (drawing_area_p),
                             &allocation);

  return FALSE;
} // drawingarea_configure_event_cb

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION(3,0,0)
#if defined (GTKGLAREA_SUPPORT)
GglaContext*
glarea_create_context_cb (GglaArea* GLArea_in,
                          gpointer userData_in)
{
  // sanity check(s)
  ACE_ASSERT (GLArea_in);
  ACE_ASSERT (userData_in);
#if GTK_CHECK_VERSION (3, 16, 0)
  ACE_ASSERT (!gtk_gl_area_get_context (GLArea_in));
#else
  ACE_ASSERT (!GLArea_in->glcontext);
#endif // GTK_CHECK_VERSION (3, 16, 0)

  GglaContext* result_p = NULL;

  GError* error_p = NULL;
  // *TODO*: this currently fails on Wayland (Gnome 3.22.24)
  // *WORKAROUND*: set GDK_BACKEND=x11 environment to force XWayland
  result_p =
#if GTK_CHECK_VERSION (4,0,0)
    gdk_surface_create_gl_context (gtk_native_get_surface (gtk_widget_get_native (GTK_WIDGET (GLArea_in))),
                                   &error_p);
#elif GTK_CHECK_VERSION (3,16,0)
    gdk_window_create_gl_context (gtk_widget_get_window (GTK_WIDGET (GLArea_in)),
                                  &error_p);
#else
    NULL;
#endif // GTK_CHECK_VERSION (3,16,0)
  if (!result_p)
  {
    ACE_ASSERT (error_p);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_window_create_gl_context(): \"%s\", aborting\n"),
                ACE_TEXT (error_p->message)));
#if GTK_CHECK_VERSION (3, 16, 0)
    gtk_gl_area_set_error (GLArea_in, error_p);
#endif // GTK_CHECK_VERSION (3, 16, 0)
    g_error_free (error_p); error_p = NULL;
    return NULL;
  } // end IF

#if GTK_CHECK_VERSION (3,16,0)
  gdk_gl_context_set_required_version (result_p,
                                       2, 1);
#if defined (_DEBUG)
  gdk_gl_context_set_debug_enabled (result_p,
                                    TRUE);
#endif // _DEBUG
  //gdk_gl_context_set_forward_compatible (result_p,
  //                                       FALSE);
  gdk_gl_context_set_use_es (result_p,
                             -1); // auto-detect
#endif // GTK_CHECK_VERSION (3,16,0)

#if GTK_CHECK_VERSION (3,16,0)
  if (!gdk_gl_context_realize (result_p,
                               &error_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to realize OpenGL context: \"%s\", continuing\n"),
                ACE_TEXT (error_p->message)));
    gtk_gl_area_set_error (GLArea_in, error_p);
    g_error_free (error_p); error_p = NULL;
    return NULL;
  } // end IF
#endif // GTK_CHECK_VERSION (3, 16, 0)

#if GTK_CHECK_VERSION (3, 16, 0)
  gdk_gl_context_make_current (result_p);
#else
  ggla_area_make_current (GLArea_in);
#endif // GTK_CHECK_VERSION (3, 16, 0)

  // initialize options
  glClearColor (0.0F, 0.0F, 0.0F, 1.0F);              // Black Background
  COMMON_GL_ASSERT;
  //glClearDepth (1.0);                                 // Depth Buffer Setup
  //COMMON_GL_ASSERT;
  /* speedups */
  //  glDisable (GL_CULL_FACE);
  //  glEnable (GL_DITHER);
  //  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  //  glHint (GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
  COMMON_GL_ASSERT;
  //glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  //COMMON_GL_ASSERT;
  //glEnable (GL_COLOR_MATERIAL);
  //COMMON_GL_ASSERT;
  //glEnable (GL_LIGHTING);
  //COMMON_GL_ASSERT;
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective
  COMMON_GL_ASSERT;
  glDepthFunc (GL_LESS);                              // The Type Of Depth Testing To Do
  COMMON_GL_ASSERT;
  glDepthMask (GL_TRUE);
  COMMON_GL_ASSERT;
  glEnable (GL_TEXTURE_2D);                           // Enable Texture Mapping
  COMMON_GL_ASSERT;
  glShadeModel (GL_SMOOTH);                           // Enable Smooth Shading
  COMMON_GL_ASSERT;
  glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  COMMON_GL_ASSERT;
  //glEnable (GL_BLEND);                                // Enable Semi-Transparency
  //COMMON_GL_ASSERT;
  //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //COMMON_GL_ASSERT;
  glEnable (GL_DEPTH_TEST);                           // Enables Depth Testing
  COMMON_GL_ASSERT;

  return result_p;
}

static Common_GL_Texture texture_c;
gboolean
glarea_render_cb (GglaArea* area_in,
                  GglaContext* context_in,
                  gpointer userData_in)
{
  // sanity check(s)
  struct Common_UI_GTK_CBData* ui_cb_data_p =
    static_cast<struct Common_UI_GTK_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);

  static bool is_first = true;
  if (is_first)
  {
    is_first = false;

    // initialize options
    glClearColor (0.0F, 0.0F, 0.0F, 1.0F);              // Black Background
    COMMON_GL_ASSERT;
    //glClearDepth (1.0);                                 // Depth Buffer Setup
    //COMMON_GL_ASSERT;
    /* speedups */
    //  glDisable (GL_CULL_FACE);
    //  glEnable (GL_DITHER);
    //  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    //  glHint (GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
//    COMMON_GL_ASSERT;
    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective
    COMMON_GL_ASSERT;
    glDepthFunc (GL_LESS);                              // The Type Of Depth Testing To Do
    COMMON_GL_ASSERT;
    glDepthMask (GL_TRUE);
    COMMON_GL_ASSERT;
    glEnable (GL_TEXTURE_2D);                           // Enable Texture Mapping
    COMMON_GL_ASSERT;
    glShadeModel (GL_SMOOTH);                           // Enable Smooth Shading
    COMMON_GL_ASSERT;
    glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    COMMON_GL_ASSERT;
    glEnable (GL_BLEND);                                // Enable Semi-Transparency
    COMMON_GL_ASSERT;
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //    glBlendFunc (GL_ONE, GL_ZERO);
    //    glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    COMMON_GL_ASSERT;
    glEnable (GL_DEPTH_TEST);                           // Enables Depth Testing
    COMMON_GL_ASSERT;

    glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    COMMON_GL_ASSERT;
    glEnable (GL_COLOR_MATERIAL);
    COMMON_GL_ASSERT;
    glEnable (GL_NORMALIZE);
    COMMON_GL_ASSERT;
    //    glEnable (GL_LIGHTING);
    //    COMMON_GL_ASSERT;

        // initialize texture
    std::string filename = Common_File_Tools::getWorkingDirectory ();
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
    filename += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
    //    filename += ACE_TEXT_ALWAYS_CHAR ("image.png");
    filename += ACE_TEXT_ALWAYS_CHAR ("crate.png");
    //    filename += ACE_TEXT_ALWAYS_CHAR ("red_alpha.png");
    if (!texture_c.load (filename))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_GL_Texture::load(\"%s\"), returning\n"),
                  ACE_TEXT (filename.c_str ())));
      return FALSE;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("OpenGL texture id: %u\n"),
                texture_c.id_));
  } // end IF

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // *TODO*: find out why this reports GL_INVALID_OPERATION
  COMMON_GL_PRINT_ERROR;

  // step1: position camera
  glLoadIdentity ();				// Reset the transformation matrix.
  COMMON_GL_ASSERT;

  glTranslatef (0.0f, 0.0f, -5.0f);		// Move back into the screen 7
  COMMON_GL_ASSERT;

  static GLfloat cube_rotation = 0.0f;
  glRotatef (cube_rotation, 0.9f, -1.0f, 1.1f);		// Rotate The Cube On X, Y, and Z
  glRotatef (cube_rotation, 1.1f, 1.0f, 0.9f);		// Rotate The Cube On X, Y, and Z
  COMMON_GL_ASSERT;

  // step2: draw cube
  ACE_ASSERT (texture_c.id_);
  texture_c.bind (0);
  Common_GL_Tools::drawCube (true);

  cube_rotation += 1.0f;					// Decrease The Rotation Variable For The Cube
  if (cube_rotation > 360.0f)
    cube_rotation -= 360.0f;

#if GTK_CHECK_VERSION (3,16,0)
  gtk_gl_area_queue_render (area_in);
#endif // GTK_CHECK_VERSION (3,16,0)

  return FALSE;
}

void
glarea_resize_cb (GglaArea* GLArea_in,
                  gint width_in,
                  gint height_in,
                  gpointer userData_in)
{
  // sanity check(s)
  ACE_ASSERT (GLArea_in);
  ACE_ASSERT (userData_in);

  //  struct Test_U_AudioEffect_UI_CBDataBase* ui_cb_data_base_p =
  //    static_cast<struct Test_U_AudioEffect_UI_CBDataBase*> (userData_in);

  //  // sanity check(s)
  //  ACE_ASSERT (ui_cb_data_base_p);

  glViewport (0, 0,
              static_cast<GLsizei> (width_in), static_cast<GLsizei> (height_in));
  // *TODO*: find out why this reports GL_INVALID_OPERATION
  COMMON_GL_PRINT_ERROR;

  glMatrixMode (GL_PROJECTION);
  COMMON_GL_ASSERT;

  glLoadIdentity ();
  COMMON_GL_ASSERT;

  gluPerspective (45.0,
                  static_cast<GLdouble> (width_in) / static_cast<GLdouble> (height_in),
                  0.1, 100.0);
  COMMON_GL_ASSERT;

  glMatrixMode (GL_MODELVIEW);
  COMMON_GL_ASSERT
}
#endif // GTKGLAREA_SUPPORT
#elif GTK_CHECK_VERSION(2,0,0)
#if defined (GTKGLAREA_SUPPORT)
GdkGLContext*
glarea_create_context_cb (GtkGLArea* GLArea_in,
                          gpointer userData_in)
{
  // sanity check(s)
  ACE_ASSERT (GLArea_in);
  ACE_ASSERT (userData_in);
#if GTK_CHECK_VERSION (3, 16, 0)
  ACE_ASSERT (!gtk_gl_area_get_context (GLArea_in));
#else
  ACE_ASSERT (!GLArea_in->glcontext);
#endif // GTK_CHECK_VERSION (3, 16, 0)

  GdkGLContext* result_p = NULL;

  GError* error_p = NULL;
  // *TODO*: this currently fails on Wayland (Gnome 3.22.24)
  // *WORKAROUND*: set GDK_BACKEND=x11 environment to force XWayland
  result_p =
#if GTK_CHECK_VERSION (4,0,0)
    gdk_surface_create_gl_context (gtk_native_get_surface (gtk_widget_get_native (GTK_WIDGET (GLArea_in))),
                                   &error_p);
#elif GTK_CHECK_VERSION (3,16,0)
    gdk_window_create_gl_context (gtk_widget_get_window (GTK_WIDGET (GLArea_in)),
                                  &error_p);
#else
    NULL;
#endif // GTK_CHECK_VERSION (3,16,0)
  if (!result_p)
  { ACE_ASSERT (error_p);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_window_create_gl_context(): \"%s\", aborting\n"),
                ACE_TEXT (error_p->message)));
#if GTK_CHECK_VERSION (3, 16, 0)
    gtk_gl_area_set_error (GLArea_in, error_p);
#endif // GTK_CHECK_VERSION (3, 16, 0)
    g_error_free (error_p); error_p = NULL;
    return NULL;
  } // end IF

#if GTK_CHECK_VERSION (3,16,0)
  gdk_gl_context_set_required_version (result_p,
                                       2, 1);
#if defined (_DEBUG)
  gdk_gl_context_set_debug_enabled (result_p,
                                    TRUE);
#endif // _DEBUG
  //gdk_gl_context_set_forward_compatible (result_p,
  //                                       FALSE);
  gdk_gl_context_set_use_es (result_p,
                             -1); // auto-detect
#endif // GTK_CHECK_VERSION (3,16,0)

#if GTK_CHECK_VERSION (3,16,0)
  if (!gdk_gl_context_realize (result_p,
                               &error_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to realize OpenGL context: \"%s\", continuing\n"),
                ACE_TEXT (error_p->message)));
    gtk_gl_area_set_error (GLArea_in, error_p);
    g_error_free (error_p); error_p = NULL;
    return NULL;
  } // end IF
#endif // GTK_CHECK_VERSION (3, 16, 0)

#if GTK_CHECK_VERSION (3, 16, 0)
  gdk_gl_context_make_current (result_p);
#else
  gdk_gl_make_current (GDK_DRAWABLE (gtk_widget_get_window (GTK_WIDGET (&GLArea_in->darea))),
                       GLArea_in->glcontext);
#endif // GTK_CHECK_VERSION (3, 16, 0)

  // initialize options
  glClearColor (0.0F, 0.0F, 0.0F, 1.0F);              // Black Background
  COMMON_GL_ASSERT;
  //glClearDepth (1.0);                                 // Depth Buffer Setup
  //COMMON_GL_ASSERT;
  /* speedups */
  //  glDisable (GL_CULL_FACE);
  //  glEnable (GL_DITHER);
  //  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  //  glHint (GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
  COMMON_GL_ASSERT;
  //glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  //COMMON_GL_ASSERT;
  //glEnable (GL_COLOR_MATERIAL);
  //COMMON_GL_ASSERT;
  //glEnable (GL_LIGHTING);
  //COMMON_GL_ASSERT;
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective
  COMMON_GL_ASSERT;
  glDepthFunc (GL_LESS);                              // The Type Of Depth Testing To Do
  COMMON_GL_ASSERT;
  glDepthMask (GL_TRUE);
  COMMON_GL_ASSERT;
  glEnable (GL_TEXTURE_2D);                           // Enable Texture Mapping
  COMMON_GL_ASSERT;
  glShadeModel (GL_SMOOTH);                           // Enable Smooth Shading
  COMMON_GL_ASSERT;
  glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  COMMON_GL_ASSERT;
  //glEnable (GL_BLEND);                                // Enable Semi-Transparency
  //COMMON_GL_ASSERT;
  //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //COMMON_GL_ASSERT;
  glEnable (GL_DEPTH_TEST);                           // Enables Depth Testing
  COMMON_GL_ASSERT;

  return result_p;
}

static Common_GL_Texture texture_c;
gboolean
glarea_render_cb (GtkGLArea* area_in,
                  GdkGLContext* context_in,
                  gpointer userData_in)
{
  // sanity check(s)
  struct Common_UI_GTK_CBData* ui_cb_data_p =
    static_cast<struct Common_UI_GTK_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);

  static bool is_first = true;
  if (is_first)
  {
    is_first = false;

    // initialize options
    glClearColor (0.0F, 0.0F, 0.0F, 1.0F);              // Black Background
    COMMON_GL_ASSERT;
    //glClearDepth (1.0);                                 // Depth Buffer Setup
    //COMMON_GL_ASSERT;
    /* speedups */
    //  glDisable (GL_CULL_FACE);
    //  glEnable (GL_DITHER);
    //  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    //  glHint (GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
//    COMMON_GL_ASSERT;
    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective
    COMMON_GL_ASSERT;
    glDepthFunc (GL_LESS);                              // The Type Of Depth Testing To Do
    COMMON_GL_ASSERT;
    glDepthMask (GL_TRUE);
    COMMON_GL_ASSERT;
    glEnable (GL_TEXTURE_2D);                           // Enable Texture Mapping
    COMMON_GL_ASSERT;
    glShadeModel (GL_SMOOTH);                           // Enable Smooth Shading
    COMMON_GL_ASSERT;
    glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    COMMON_GL_ASSERT;
    glEnable (GL_BLEND);                                // Enable Semi-Transparency
    COMMON_GL_ASSERT;
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFunc (GL_ONE, GL_ZERO);
//    glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    COMMON_GL_ASSERT;
    glEnable (GL_DEPTH_TEST);                           // Enables Depth Testing
    COMMON_GL_ASSERT;

    glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    COMMON_GL_ASSERT;
    glEnable (GL_COLOR_MATERIAL);
    COMMON_GL_ASSERT;
    glEnable (GL_NORMALIZE);
    COMMON_GL_ASSERT;
//    glEnable (GL_LIGHTING);
//    COMMON_GL_ASSERT;

    // initialize texture
    std::string filename = Common_File_Tools::getWorkingDirectory ();
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
    filename += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
//    filename += ACE_TEXT_ALWAYS_CHAR ("image.png");
    filename += ACE_TEXT_ALWAYS_CHAR ("crate.png");
//    filename += ACE_TEXT_ALWAYS_CHAR ("red_alpha.png");
    if (!texture_c.load (filename))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_GL_Texture::load(\"%s\"), returning\n"),
                  ACE_TEXT (filename.c_str ())));
      return FALSE;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("OpenGL texture id: %u\n"),
                texture_c.id_));
  } // end IF

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // *TODO*: find out why this reports GL_INVALID_OPERATION
  COMMON_GL_PRINT_ERROR;

  // step1: position camera
  glLoadIdentity ();				// Reset the transformation matrix.
  COMMON_GL_ASSERT;

  glTranslatef (0.0f, 0.0f, -5.0f);		// Move back into the screen 7
  COMMON_GL_ASSERT;

  static GLfloat cube_rotation = 0.0f;
  glRotatef (cube_rotation, 0.9f, -1.0f, 1.1f);		// Rotate The Cube On X, Y, and Z
  glRotatef (cube_rotation, 1.1f, 1.0f, 0.9f);		// Rotate The Cube On X, Y, and Z
  COMMON_GL_ASSERT;

  // step2: draw cube
  ACE_ASSERT (texture_c.id_);
  texture_c.bind (0);
  Common_GL_Tools::drawCube (true);

  cube_rotation += 1.0f;					// Decrease The Rotation Variable For The Cube
  if (cube_rotation > 360.0f)
    cube_rotation -= 360.0f;

#if GTK_CHECK_VERSION (3,16,0)
  gtk_gl_area_queue_render (area_in);
#endif // GTK_CHECK_VERSION (3,16,0)

  return FALSE;
}

void
glarea_resize_cb (GtkGLArea* GLArea_in,
                  gint width_in,
                  gint height_in,
                  gpointer userData_in)
{
  // sanity check(s)
  ACE_ASSERT (GLArea_in);
  ACE_ASSERT (userData_in);

//  struct Test_U_AudioEffect_UI_CBDataBase* ui_cb_data_base_p =
//    static_cast<struct Test_U_AudioEffect_UI_CBDataBase*> (userData_in);

//  // sanity check(s)
//  ACE_ASSERT (ui_cb_data_base_p);

  glViewport (0, 0,
              static_cast<GLsizei> (width_in), static_cast<GLsizei> (height_in));
  // *TODO*: find out why this reports GL_INVALID_OPERATION
  COMMON_GL_PRINT_ERROR;

  glMatrixMode (GL_PROJECTION);
  COMMON_GL_ASSERT;

  glLoadIdentity ();
  COMMON_GL_ASSERT;

  gluPerspective (45.0,
                  static_cast<GLdouble> (width_in) / static_cast<GLdouble> (height_in),
                  0.1, 100.0);
  COMMON_GL_ASSERT;

  glMatrixMode (GL_MODELVIEW);
  COMMON_GL_ASSERT
}
#endif // GTKGLAREA_SUPPORT
#endif // GTK_CHECK_VERSION(x,0,0)
#endif // GTKGL_SUPPORT

#if GTK_CHECK_VERSION(4,0,0)
void
filechooserdialog_cb (GtkNativeDialog*,
                      int)
{

}
#else
void
filechooserbutton_cb (GtkFileChooserButton* fileChooserButton_in,
                      gpointer userData_in)
{
  // sanity check(s)
  ACE_ASSERT (fileChooserButton_in);
  struct Common_UI_GTK_CBData* ui_cb_data_p =
    static_cast<struct Common_UI_GTK_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

  GFile* file_p =
    gtk_file_chooser_get_file (GTK_FILE_CHOOSER (fileChooserButton_in));
  if (!file_p)
    return; // nothing selected (yet)
  char* string_p = g_file_get_path (file_p);
  if (!string_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_file_get_path(%@): \"%m\", returning\n"),
                file_p));
    g_object_unref (G_OBJECT (file_p)); file_p = NULL;
    return;
  } // end IF
  g_object_unref (G_OBJECT (file_p)); file_p = NULL;
  std::string filename_string =
    Common_UI_GTK_Tools::UTF8ToLocale (string_p, -1);
  if (filename_string.empty ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_GTK_Tools::UTF8ToLocale(\"%s\"): \"%m\", returning\n"),
                ACE_TEXT (string_p)));
    g_free (string_p); string_p = NULL;
    return;
  } // end IF
  g_free (string_p); string_p = NULL;
} // filechooserbutton_cb
//void
//filechooserdialog_cb (GtkFileChooser* fileChooser_in,
//                      gpointer userData_in)
//{
//  STREAM_TRACE (ACE_TEXT ("::filechooserdialog_cb"));
//
//  ACE_UNUSED_ARG (userData_in);
//
//  gtk_dialog_response (GTK_DIALOG (GTK_FILE_CHOOSER_DIALOG (fileChooser_in)),
//                       GTK_RESPONSE_ACCEPT);
//} // filechooserdialog_cb
#endif // GTK_CHECK_VERSION(4,0,0)

gboolean
key_cb (GtkWidget* widget_in,
        GdkEvent* event_in,
        gpointer userData_in)
{
  ACE_UNUSED_ARG (widget_in);

  // sanity check(s)
  ACE_ASSERT (event_in);

  struct Common_UI_GTK_CBData* ui_cb_data_p =
      reinterpret_cast<struct Common_UI_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (ui_cb_data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

#if GTK_CHECK_VERSION(4,0,0)
  switch (gdk_key_event_get_keyval (event_in))
#else
  switch (event_in->key.keyval)
#endif // GTK_CHECK_VERSION(4,0,0)
  {
#if GTK_CHECK_VERSION(3,0,0)
    case GDK_KEY_Escape:
    case GDK_KEY_f:
    case GDK_KEY_F:
#else
    case GDK_Escape:
    case GDK_f:
    case GDK_F:
#endif // GTK_CHECK_VERSION(3,0,0)
    {
      bool is_active_b = false;
      GtkToggleButton* toggle_button_p =
        GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (TEST_I_UI_GTK_TOGGLEBUTTON_RECORD_NAME)));
      ACE_ASSERT (toggle_button_p);
      is_active_b = gtk_toggle_button_get_active (toggle_button_p);

      // sanity check(s)
#if GTK_CHECK_VERSION(4,0,0)
      if ((gdk_key_event_get_keyval (eventKey_in) == GDK_KEY_Escape) &&
#elif GTK_CHECK_VERSION(3,0,0)
      if ((eventKey_in->keyval == GDK_KEY_Escape) &&
#else
      if ((event_in->key.keyval == GDK_Escape) &&
#endif // GTK_CHECK_VERSION(3/4,0,0)
          !is_active_b)
        break; // <-- not in fullscreen mode, nothing to do

      gtk_toggle_button_set_active (toggle_button_p,
                                    !is_active_b);

      break;
    }
    default:
      return FALSE; // propagate
  } // end SWITCH

  return TRUE; // done (do not propagate further)
}

gboolean
drawingarea_key_press_event_cb (GtkWidget* widget_in,
                                GdkEvent* event_in,
                                gpointer userData_in)
{
  return key_cb (widget_in, event_in, userData_in);
}

gboolean
dialog_main_key_press_event_cb (GtkWidget* widget_in,
                                GdkEvent* event_in,
                                gpointer userData_in)
{
  return key_cb (widget_in, event_in, userData_in);
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
