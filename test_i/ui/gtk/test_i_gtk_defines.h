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

#ifndef TEST_I_GTK_DEFINES_H
#define TEST_I_GTK_DEFINES_H

#if (GTK2_USE)
#define TEST_U_UI_DEFINITION_FILE                      "gtk_ui.gtk2"
#elif (GTK3_USE)
#define TEST_U_UI_DEFINITION_FILE                      "gtk_ui.gtk3"
#elif (GTK4_USE)
#define TEST_U_UI_DEFINITION_FILE                      "gtk_ui.gtk4"
#else
#define TEST_U_UI_DEFINITION_FILE                      "gtk_ui.glade"
#endif // GTKX_USE

#define TEST_I_UI_GTK_BUTTON_ABOUT_NAME                "about"
#define TEST_I_UI_GTK_BUTTON_QUIT_NAME                 "quit"
#define TEST_I_UI_GTK_COMBOBOX_SOURCE_NAME             "combobox_source"
#define TEST_I_UI_GTK_COMBOBOX_SOURCE_2_NAME           "combobox_source_2"
#define TEST_I_UI_GTK_DIALOG_ABOUT_NAME                "dialog_about"
#define TEST_I_UI_GTK_DIALOG_MAIN_NAME                 "dialog_main"
#define TEST_I_UI_GTK_DRAWINGAREA_NAME                 "drawingarea"
#define TEST_I_UI_GTK_FILECHOOSERBUTTON_SAVE_NAME      "filechooserbutton_save"
//#define TEST_I_UI_GTK_FILECHOOSERDIALOG_SAVE_NAME     "filechooserdialog_save"
#define TEST_I_UI_GTK_LISTSTORE_SOURCE_NAME            "liststore_source"
#define TEST_I_UI_GTK_LISTSTORE_SOURCE_2_NAME          "liststore_source_2"

//#define TEST_I_UI_GTK_PANGO_LOG_FONT_DESCRIPTION       "Monospace 8"
//#define TEST_I_UI_GTK_PANGO_LOG_COLOR_BASE             "#FFFFFF" // white
//#define TEST_I_UI_GTK_PANGO_LOG_COLOR_TEXT             "#000000" // green
#define TEST_I_UI_GTK_PROGRESSBAR_NAME                 "progressbar"
#define TEST_I_UI_GTK_STATUSBAR_NAME                   "statusbar"
#define TEST_I_UI_GTK_TOGGLEBUTTON_RECORD_NAME         "togglebutton_record"

#endif
